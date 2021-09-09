#define _CRT_SECURE_NO_WARNINGS

#include "App.hpp"
#include "base/Main.hpp"
#include "gpu/GLContext.hpp"
#include "gpu/Buffer.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>

#include "integrators.hpp"

using namespace FW;
using namespace std;

namespace {

	enum VertexShaderAttributeLocations
	{
		ATTRIB_POSITION = 0,
		ATTRIB_NORMAL = 1
	};

} // namespace

App::App(void)
	: common_ctrl_(CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5),
	system_changed_(false),
	shading_toggle_(false),
	shading_mode_changed_(false),
	wind_force_toggle_(false),
	wind_force_changed_(false),
	frictionless_collisions_toggle_(false),
	frictionless_collisions_changed_(false),
	camera_rotation_angle_(0.0f),
	step_(0.0001f),
	steps_per_update_(5),
	integrator_(MIDPOINT_INTEGRATOR),
	ps_type_(SIMPLE_SYSTEM),
	ps_(&simple_system_),
	timer_(true),
	simple_system_(),
	spring_system_(),
	pendulum_system_(10),
	cloth_system_(10, 10),
	sprinkler_system_(200),
	initial_implicit_(false)
{
	static_assert(is_standard_layout<Vertex>::value, "struct Vertex must be standard layout to use offsetof");
	initRendering();

	common_ctrl_.showFPS(true);
	common_ctrl_.addToggle((S32*)&ps_type_, SIMPLE_SYSTEM, FW_KEY_1, "Simple system (1)", &system_changed_);
	common_ctrl_.addToggle((S32*)&ps_type_, SPRING_SYSTEM, FW_KEY_2, "R2 Spring system (2)", &system_changed_);
	common_ctrl_.addToggle((S32*)&ps_type_, PENDULUM_SYSTEM, FW_KEY_3, "R4 Pendulum system (3)", &system_changed_);
	common_ctrl_.addToggle((S32*)&ps_type_, CLOTH_SYSTEM, FW_KEY_4, "R5 Cloth system (4)", &system_changed_);
	common_ctrl_.addToggle((S32*)&ps_type_, SPRINKLER_SYSTEM, FW_KEY_S, "Extra Sprinkler system (5)", &system_changed_);
#ifdef COMPUTE_CLOTH_MODULE
	common_ctrl_.addToggle((S32*)&ps_type_, COMPUTE_CLOTH, FW_KEY_NONE, "EXTRA: Compute cloth", &system_changed_);
#endif

	common_ctrl_.addSeparator();

	common_ctrl_.addToggle((S32*)&integrator_, EULER_INTEGRATOR, FW_KEY_5, "R1 Euler integrator (5)");
	common_ctrl_.addToggle((S32*)&integrator_, TRAPEZOID_INTEGRATOR, FW_KEY_6, "R3 Trapezoid integrator (6)");
	common_ctrl_.addToggle((S32*)&integrator_, MIDPOINT_INTEGRATOR, FW_KEY_7, "Midpoint integrator (7)");
	common_ctrl_.addToggle((S32*)&integrator_, RK4_INTEGRATOR, FW_KEY_8, "EXTRA: RK4 integrator (8)");
	common_ctrl_.addToggle((S32*)&integrator_, IMPLICIT_EULER_INTEGRATOR, FW_KEY_9, "EXTRA: Implicit Euler integrator (9)");
	common_ctrl_.addToggle((S32*)&integrator_, IMPLICIT_MIDPOINT_INTEGRATOR, FW_KEY_0, "EXTRA: Implicit midpoint integrator (0)");
	common_ctrl_.addToggle((S32*)&integrator_, CRANK_NICOLSON_INTEGRATOR, FW_KEY_PLUS, "EXTRA: Crank-Nicolson integrator (+)");
#ifdef COMPUTE_CLOTH_MODULE
	common_ctrl_.addToggle((S32*)&integrator_, COMPUTE_CLOTH_INTEGRATOR, FW_KEY_NONE, "EXTRA: Compute integrator for cloth");
#endif
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle(&shading_toggle_, FW_KEY_T, "Toggle cloth rendering mode (T)", &shading_mode_changed_);
	common_ctrl_.addToggle(&wind_force_toggle_, FW_KEY_W, "Toggle wind force for cloth (W)", &wind_force_changed_);
	common_ctrl_.addToggle(&frictionless_collisions_toggle_, FW_KEY_F, "Frictionless collisions (F)", &frictionless_collisions_changed_);
#ifdef COMPUTE_CLOTH_MODULE
	common_ctrl_.addButton(&fireBullet, FW_KEY_SPACE, "EXTRA: Fire bullet from mouse position (SPACE)");
#endif
	common_ctrl_.beginSliderStack();
	common_ctrl_.addSlider((F32*)&step_, 0.00001, 0.05, true, FW_KEY_NONE, FW_KEY_NONE, "Step size: %.4f", 0.0f);
	common_ctrl_.addSlider((S32*)&steps_per_update_, 1, 100, false, FW_KEY_NONE, FW_KEY_NONE, "Steps per update: %d");
	common_ctrl_.endSliderStack();
	window_.setTitle("Assignment 3");

	window_.addListener(this);
	window_.addListener(&common_ctrl_);

	window_.setSize(Vec2i(800, 800));
	ps_->reset();

#ifdef COMPUTE_CLOTH_MODULE
	compute_cloth_ = ComputeCloth(window_.getGL());
#endif

#ifdef EIGEN_SPARSECORE_MODULE_H
	ps_J_ = Eigen::SparseMatrix<float>(ps_->state().size() * 3, ps_->state().size() * 3);
#endif
}

bool App::handleEvent(const Window::Event& ev) {
	if (system_changed_) {
		system_changed_ = false;
		switch (ps_type_) {
		case SIMPLE_SYSTEM:
			ps_ = &simple_system_; break;
		case SPRING_SYSTEM:
			ps_ = &spring_system_; break;
		case PENDULUM_SYSTEM:
			ps_ = &pendulum_system_; break;
		case CLOTH_SYSTEM:
			ps_ = &cloth_system_; break;
		case SPRINKLER_SYSTEM:
			ps_ = &sprinkler_system_; break;
#ifdef COMPUTE_CLOTH_MODULE
		case COMPUTE_CLOTH:
			integrator_ = COMPUTE_CLOTH_INTEGRATOR;
			compute_cloth_.Reset();	// Reset separately since ComputeCloth doesn't inherit from ParticleSystem
			break;
#endif
		default:
			assert(false && "invalid system type");
		}
		ps_->reset();
		initial_implicit_ = true;
#ifdef EIGEN_SPARSECORE_MODULE_H
		ps_J_ = SparseMatrix(ps_->state().size() * 3, ps_->state().size() * 3);
#endif
	}

	if (shading_mode_changed_) {
		common_ctrl_.message(shading_toggle_ ? "EXTRA: Surface rendering" : "Wireframe rendering");
		shading_mode_changed_ = false;
	}

	if(wind_force_changed_)
	{
		cloth_system_.setWindForceToggle(wind_force_toggle_);
		wind_force_changed_ = false;
	}

	if(frictionless_collisions_changed_)
	{
		cloth_system_.setFrictionlessCollisionsToggle(frictionless_collisions_toggle_);
		frictionless_collisions_changed_ = false;
	}

	
	if (ev.type == Window::EventType_KeyDown) {
		if (ev.key == FW_KEY_HOME)
			camera_rotation_angle_ -= 0.05 * FW_PI;
		else if (ev.key == FW_KEY_END)
			camera_rotation_angle_ += 0.05 * FW_PI;

		if(ev.key == FW_KEY_R)
		{
			ps_->reset();
		}
	}
	

	if (ev.type == Window::EventType_Mouse) {
		if (ev.mouseDragging)
			camera_rotation_angle_ += 0.01f * ev.mouseDelta.x;
	}

	if (ev.type == Window::EventType_Close) {
		window_.showModalMessage("Exiting...");
		delete this;
		return true;
	}

	if (fireBullet)
	{
		// YOUR CODE HERE (bullet extra):
		// Compute bullet origin and direction here based on current view matrix and mouse position
	}

	window_.setVisible(true);
	if (ev.type == Window::EventType_Paint)
	{
		if (integrator_ != previous_integrator_ || step_ != previous_step_) {
			initial_implicit_ = true;
#ifdef EIGEN_SPARSECORE_MODULE_H
			ps_J_ = SparseMatrix(ps_->state().size() * 3, ps_->state().size() * 3);
#endif
		}

		for (int i = 0; i < steps_per_update_; ++i) {
			switch (integrator_) {
			case EULER_INTEGRATOR:
				eulerStep(*ps_, step_); break;
			case TRAPEZOID_INTEGRATOR:
				trapezoidStep(*ps_, step_); break;
			case MIDPOINT_INTEGRATOR:
				midpointStep(*ps_, step_); break;
			case RK4_INTEGRATOR:
				rk4Step(*ps_, step_); break;
#ifdef EIGEN_SPARSECORE_MODULE_H
			case IMPLICIT_EULER_INTEGRATOR:
				implicit_euler_step(*ps_, step_, ps_J_, implicit_solver_, initial_implicit_); break;
			case IMPLICIT_MIDPOINT_INTEGRATOR:
				implicit_midpoint_step(*ps_, step_, ps_J_, implicit_solver_, initial_implicit_); break;
			case CRANK_NICOLSON_INTEGRATOR:
				crank_nicolson_step(*ps_, step_, ps_J_, implicit_solver_, initial_implicit_); break;
#endif
#ifdef COMPUTE_CLOTH_MODULE
			case COMPUTE_CLOTH_INTEGRATOR:
				compute_cloth_.Advance(step_); break;
#endif
			default:
				assert(false && " invalid integrator type");
			}
			initial_implicit_ = false;
		}
		previous_integrator_ = integrator_;
		previous_step_ = step_;

		render();
	}

	window_.repaint();

	return false;
}

void App::initRendering() {
	// Ask the Nvidia framework for the GLContext object associated with the window.
	// As a side effect, this initializes the OpenGL context and lets us call GL functions.
	auto ctx = window_.getGL();

	glGenBuffers(1, &gl_.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.vertex_buffer);

	// Mesh rendering setup
	glGenVertexArrays(1, &gl_.mesh_vao);
	glBindVertexArray(gl_.mesh_vao);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Point and line rendering setup
	glGenVertexArrays(1, &gl_.point_vao);
	glBindVertexArray(gl_.point_vao);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (GLvoid*)0);
	glBindVertexArray(0);

	auto shader_program = new GLContext::Program(
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
			layout(location = 0) in vec4 aPosition;
	layout(location = 1) in vec3 aNormal;

	out vec4 vColor;

	uniform mat4 uWorldToClip;

	const vec3 directionToLight = normalize(vec3(0.5, 0.3, 0.6));

	void main()
	{
		float clampedCosine = clamp(dot(aNormal, directionToLight), 0.0, 1.0);
		vec3 litColor = vec3(clampedCosine);
		gl_Position = uWorldToClip * aPosition;
		vColor = vec4(litColor, 1);
	}
	),
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
			in vec4 vColor;
	out vec4 fColor;
	void main()
	{
		fColor = vColor;
	}
	));
	ctx->setProgram("shaders", shader_program);

	// Get the IDs of the shader program and its uniform input locations from OpenGL.
	gl_.shader_program = shader_program->getHandle();
	gl_.world_to_clip_uniform = glGetUniformLocation(gl_.shader_program, "uWorldToClip");
}

void App::render()
{
	// Clear screen.
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Our camera orbits around origin at a fixed distance.
	static const float cameraDistance = 2.1f;
	Mat4f C;
	Mat3f rot = Mat3f::rotation(Vec3f(0, 1, 0), -camera_rotation_angle_);
	C.setCol(0, Vec4f(rot.getCol(0), 0));
	C.setCol(1, Vec4f(rot.getCol(1), 0));
	C.setCol(2, Vec4f(rot.getCol(2), 0));
	C.setCol(3, Vec4f(0, 0, cameraDistance, 1));

	// Simple perspective.
	Mat4f P;
	static const float fNear = 0.1f, fFar = 4.0f;
	P.setCol(0, Vec4f(1, 0, 0, 0));
	P.setCol(1, Vec4f(0, 1, 0, 0));
	P.setCol(2, Vec4f(0, 0, (fFar + fNear) / (fFar - fNear), 1));
	P.setCol(3, Vec4f(0, 0, -2 * fFar * fNear / (fFar - fNear), 0));

	Mat4f worldToClip = P * C;

	if (ps_type_ == CLOTH_SYSTEM && shading_toggle_) {
		// EXTRA: Render cloth surface.

	}
#ifdef COMPUTE_CLOTH_MODULE
	else if (ps_type_ == COMPUTE_CLOTH)
		compute_cloth_.Render(shading_toggle_, worldToClip);
#endif
	else {
		// Draw particle system as lines and points.
		glUseProgram(0);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&C(0, 0));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&P(0, 0));

		glBindVertexArray(gl_.point_vao);
		glBindBuffer(GL_ARRAY_BUFFER, gl_.vertex_buffer);
		auto p = ps_->getPoints();
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * p.size(), p.data(), GL_STATIC_DRAW);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, p.size());

		auto l = ps_->getLines();
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * l.size(), l.data(), GL_STATIC_DRAW);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, l.size());
	}

	// Undo our bindings.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// Check for OpenGL errors.
	GLContext::checkErrors();

	common_ctrl_.message(sprintf("Home/End or drag mouse to rotate camera.\nR to restart"), "instructions");
	
}

void FW::init(void) {
	new App;
}
