
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

#include "objects.hpp"
#include "camera.h"

#include "raytracer.hpp"

using namespace FW;
using namespace std;

App::App(void)
	: common_ctrl_(CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5),
	camera_type_(PERSPECTIVE_CAMERA),
	camera_position_(.0f, .0f, 2.0f),
	camera_velocity_(.0f),
	camera_rotation_(.0f),
	camera_speed_(.001f),
	mouse_right_down_(false),
	load_scene_(false),
	raytrace_(false),
	display_results_(false),
	fov_(1.0f),
	ortho_size_(10.0f),
	shadows_(true),
	transparent_shadows_(true),
	shade_back_(true),
	sampler_type_(UNIFORM_SAMPLER),
	sample_count_(1),
	downscale_factor_(16),
	bounces_(3),
	result_(0),
	display_uv_(false)
{
	initRendering();

	common_ctrl_.showFPS(true);
	common_ctrl_.addButton(&load_scene_, FW_KEY_M, "Load Scene(M)");
	common_ctrl_.addButton(&raytrace_, FW_KEY_ENTER, "Raytrace(Enter)", &display_results_);
	common_ctrl_.addToggle(&display_results_, FW_KEY_SPACE, "Display results(Space)");
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle((S32*)&camera_type_, ORTHO_CAMERA, FW_KEY_1, "Orthogonal camera (1)");
	common_ctrl_.addToggle((S32*)&camera_type_, PERSPECTIVE_CAMERA, FW_KEY_2, "Perspective camera(2)");
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle((S32*)&sampler_type_, UNIFORM_SAMPLER, FW_KEY_3, "Uniform AA sampling (3)");
	common_ctrl_.addToggle((S32*)&sampler_type_, REGULAR_SAMPLER, FW_KEY_4, "Regular AA sampling (4)");
	common_ctrl_.addToggle((S32*)&sampler_type_, JITTERED_SAMPLER, FW_KEY_5, "Jittered AA sampling (5)");
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle(&shadows_, FW_KEY_6, "Shadows enabled (6)");
	common_ctrl_.addToggle(&transparent_shadows_, FW_KEY_7, "Transparent shadows (7)");
	common_ctrl_.addToggle(&shade_back_, FW_KEY_8, "Shade backsides (8)");
	common_ctrl_.addToggle(&display_uv_, FW_KEY_0, "Display image UV (0)");
	common_ctrl_.addSeparator();
	//common_ctrl_.addToggle(&shading_toggle_,							FW_KEY_T, "Toggle cloth rendering mode (T)", &shading_mode_changed_);
	common_ctrl_.beginSliderStack();
	common_ctrl_.addSlider((S32*)&sample_count_, 1, 16, false, FW_KEY_NONE, FW_KEY_NONE, "Multisampling: %d", .0f);
	common_ctrl_.addSlider((S32*)&downscale_factor_, 1, 64, false, FW_KEY_NONE, FW_KEY_NONE, "Downscale factor: %d", .0f);
	common_ctrl_.addSlider((S32*)&bounces_, 1, 16, false, FW_KEY_NONE, FW_KEY_NONE, "Bounces: %d", .0f);
	common_ctrl_.endSliderStack();
	common_ctrl_.beginSliderStack();
	common_ctrl_.addSlider((F32*)&camera_speed_, 0.0001, 0.1, true, FW_KEY_NONE, FW_KEY_NONE, "Camera speed: %.4f", .0f);
	common_ctrl_.addSlider((F32*)&fov_, 0.0001, FW_PI*.9, true, FW_KEY_NONE, FW_KEY_NONE, "FOV: %.4f", .0f);
	common_ctrl_.addSlider((F32*)&ortho_size_, 0.1, 50.0, true, FW_KEY_NONE, FW_KEY_NONE, "Size: %.4f", .0f);
	common_ctrl_.endSliderStack();
	window_.setTitle("Assignment 5");

	window_.getGL()->swapBuffers();

	window_.addListener(this);
	window_.addListener(&common_ctrl_);

	window_.setSize(Vec2i(800, 800));

	scene_.reset(new SceneParser());
	load_scene_ = true;
}

bool App::handleEvent(const Window::Event& ev) {

	if (load_scene_) {
		auto filename_ = window_.showFileLoadDialog("Load scene");
		if (filename_.getLength()) {
			scene_.reset(new SceneParser(filename_.getPtr()));
			scene_camera_rotation_ = scene_->getCamera()->getOrientation();
			
			FW::Vec3f direction = scene_camera_rotation_.getCol(2);

			camera_position_ = scene_->getCamera()->getCenter();
			if (scene_->getCamera()->isOrtho()) {
				ortho_size_ = ((OrthographicCamera*)scene_->getCamera())->getSize();
				camera_type_ = ORTHO_CAMERA;
			}
			else {
				fov_ = ((PerspectiveCamera*)scene_->getCamera())->getFov();
				camera_type_ = PERSPECTIVE_CAMERA;
			}
			camera_rotation_ = Vec2f(.0f);
		}
		load_scene_ = false;
		display_results_ = false;
	}

	if (raytrace_) {

		copyCamera();

		SceneParser& local_scene(*scene_.get());
		auto args = get_args();
		auto tr = RayTracer(local_scene, args);

		// If there is no scene, just display the UV coords
		if (!local_scene.getGroup())
			args.display_uv = true;

		result_ = ::render(tr, local_scene, args);

		raytrace_ = false;
	}

	if (ev.type == Window::EventType_KeyDown) {
		if (ev.key == FW_KEY_W)	camera_velocity_.z = +1.0f;
		if (ev.key == FW_KEY_S)	camera_velocity_.z = -1.0f;
		if (ev.key == FW_KEY_A)	camera_velocity_.x = -1.0f;
		if (ev.key == FW_KEY_D)	camera_velocity_.x = +1.0f;
		if (ev.key == FW_KEY_WHEEL_UP)   camera_speed_ *= 1.2f;
		if (ev.key == FW_KEY_WHEEL_DOWN) camera_speed_ /= 1.2f;
		if (ev.key == FW_KEY_MOUSE_RIGHT) mouse_right_down_ = true;
		if (ev.key == FW_KEY_R) scene_camera_rotation_.setIdentity();

		if (ev.key == FW_KEY_MOUSE_LEFT && display_results_) {

			copyCamera();

			auto args = get_args();
			SceneParser& local_scene(*scene_.get());
			auto tr = RayTracer(local_scene, args, true);
			Vec2f ray_xy = Camera::normalizedImageCoordinateFromPixelCoordinate(ev.mousePos, window_.getSize());
			Ray r = scene_->getCamera()->generateRay(ray_xy);
			Hit hit;
			float tmin = scene_->getCamera()->getTMin();
			Vec3f sample_color = tr.traceRay(r, tmin, args.bounces, 1.0f, hit, Vec3f(1.0f));

			debug_rays = tr.debug_rays;

			display_results_ = false;
		}
	}

	if (ev.type == Window::EventType_KeyUp) {
		if (ev.key == FW_KEY_W)	camera_velocity_.z = .0f;
		if (ev.key == FW_KEY_S)	camera_velocity_.z = .0f;
		if (ev.key == FW_KEY_A)	camera_velocity_.x = .0f;
		if (ev.key == FW_KEY_D)	camera_velocity_.x = .0f;
		if (ev.key == FW_KEY_MOUSE_RIGHT) mouse_right_down_ = false;
	}

	if (ev.type == Window::EventType_Mouse) {
		if (ev.mouseDragging && mouse_right_down_) {
			//scene_camera_rotation_.setIdentity();
			camera_rotation_ += Vec2f(ev.mouseDelta)*.01f;
		}
	}

	if (ev.type == Window::EventType_Close) {
		window_.showModalMessage("Exiting...");
		delete this;
		return true;
	}

	window_.setVisible(true);
	if (ev.type == Window::EventType_Paint)
		render();

	window_.repaint();

	return false;
}

Mat4f App::getCamera(void) {

	Mat3f rot = Mat3f::rotation(Vec3f(1.0f, .0f, .0f), camera_rotation_.y) * Mat3f::rotation(Vec3f(.0f, 1.0f, .0f), camera_rotation_.x);
	rot = scene_camera_rotation_.transposed() * rot * scene_camera_rotation_ * scene_camera_rotation_;

	Mat4f C;
	C.setCol(0, Vec4f(rot.getCol(0), .0f));
	C.setCol(1, Vec4f(rot.getCol(1), .0f));
	C.setCol(2, Vec4f(rot.getCol(2), .0f));

	camera_position_ += C.transposed() * camera_velocity_ * camera_speed_;

	C = C * Mat4f::translate(-camera_position_);
	
	return C;
}

void App::copyCamera(void){

	auto C = getCamera();

	auto c = scene_->getCamera();
	if (c == nullptr || c->isOrtho() != (camera_type_ == ORTHO_CAMERA)) {
		delete c;
		if (camera_type_ == ORTHO_CAMERA)
			scene_->setCamera(new OrthographicCamera(Vec3f(), Vec3f(), Vec3f(), .0f));
		else
			scene_->setCamera(new PerspectiveCamera(Vec3f(), Vec3f(), Vec3f(), .0f));
	}

	scene_->getCamera()->setOrientation(C.getXYZ().transposed());
	scene_->getCamera()->setCenter(camera_position_);

	if (scene_->getCamera()->isOrtho())
		((OrthographicCamera*)scene_->getCamera())->setSize(ortho_size_);
	else
		((PerspectiveCamera*)scene_->getCamera())->setFov(fov_);

}

Args App::get_args() {
	
	Args args;

	args.shadows = shadows_;
	args.transparent_shadows = transparent_shadows_;
	args.shade_back = shade_back_;
	args.display_uv = display_uv_;
	args.bounces = bounces_;
	args.width = window_.getSize().x / downscale_factor_;
	args.height = window_.getSize().y / downscale_factor_;
	args.reconstruction_filter = Args::ReconstructionFilterType::Filter_Box;
	args.sampling_pattern = (Args::SamplePatternType)sampler_type_;
	args.num_samples = sample_count_;
	args.output_file = "debug.png";
	args.depth_min = .0f;
	args.depth_max = 1000.0f;
	args.show_progress = true;

	return args;
}

void App::initRendering() {
	// Ask the Nvidia framework for the GLContext object associated with the window.
	// As a side effect, this initializes the OpenGL context and lets us call GL functions.
	window_.getGL();

}

void App::render()
{

	// Clear screen.
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (display_results_) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, result_);

		window_.getGL()->drawTexture(0, Vec4f(-1.0f, -1.0f, .0f, 1.0f), Vec2f(1.0f, 1.0f), Vec2f(0.0f, 1.0f), Vec2f(1.0f, .0f));

		return;
	}

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// Our camera orbits around origin at a fixed distance.

	auto C = getCamera();

	Mat4f P;
	const float fNear = 0.02f, fFar = 80.0f;
	auto window_size = Vec2f(window_.getSize());
	if (camera_type_ == PERSPECTIVE_CAMERA) {
		const float near_width = 1.0f / tan(fov_ / 2.0f);
		P.setCol(0, Vec4f(near_width, 0, 0, 0));
		P.setCol(1, Vec4f(0, near_width, 0, 0));
		P.setCol(2, Vec4f(0, 0, (fFar + fNear) / (fFar - fNear), 1));
		P.setCol(3, Vec4f(0, 0, -2.0f * fFar*fNear / (fFar - fNear), 0));
	}
	else if (camera_type_ == ORTHO_CAMERA) {
		P.setCol(0, Vec4f(2.0f/ortho_size_, 0, 0, 0));
		P.setCol(1, Vec4f(0, 2.0f/ortho_size_, 0, 0));
		P.setCol(2, Vec4f(0, 0, 2.0f / (fFar - fNear), .0f));
		P.setCol(3, Vec4f(0, 0, -(fFar + fNear) / (fFar - fNear), 1));
	}

	if (scene_ != nullptr) {
		Group* group = scene_->getGroup();
		if(group != nullptr)
			scene_->getGroup()->preview_render(window_.getGL(), C, P);
		
	}

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION); glLoadMatrixf(P.getPtr());
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(C.getPtr());

	glBegin(GL_LINES);
	for (auto& r : debug_rays) {
		auto p = [](Vec3f a){ glVertex3f(a.x, a.y, a.z); };
		glColor4f(r.color.x, r.color.y, r.color.z, .8f);
		p(r.origin);
		p(r.origin + r.offset);
		glColor4f(.0f, .0f, 1.0f, .8f);
		p(r.origin + r.offset);
		p(r.origin + r.offset+r.normal_at_offset*.1f);
	}
	glEnd();

	common_ctrl_.message(sprintf("Home/End or drag mouse to rotate camera."), "instructions");
}

void FW::init(void) {
	new App;
}
