#pragma once

#include "particle_systems.hpp"

#include "gui/Window.hpp"
#include "gui/CommonControls.hpp"

#include <vector>

// Uncomment this include if you want to do the compute cloth extra.
//#include "ComputeCloth.hpp"

namespace FW {

struct Vertex
{
	Vec3f position;
	Vec3f normal;
};

struct glGeneratedIndices
{
	GLuint point_vao, mesh_vao;
	GLuint shader_program;
	GLuint vertex_buffer;
	GLuint model_to_world_uniform, world_to_clip_uniform;
};

class App : public Window::Listener
{
private:
	enum ParticleSystemType {
		SIMPLE_SYSTEM,
		SPRING_SYSTEM,
		PENDULUM_SYSTEM,
		CLOTH_SYSTEM,
		COMPUTE_CLOTH
	};
	enum IntegratorType {
		EULER_INTEGRATOR,
		TRAPEZOID_INTEGRATOR,
		MIDPOINT_INTEGRATOR,
		RK4_INTEGRATOR,
		IMPLICIT_EULER_INTEGRATOR,
		IMPLICIT_MIDPOINT_INTEGRATOR,
		CRANK_NICOLSON_INTEGRATOR,
		COMPUTE_CLOTH_INTEGRATOR
	};
public:
					App             (void);
	virtual bool    handleEvent     (const Window::Event& ev);

private:
	void			initRendering		(void);
	void			render				(void);

private:
					App             (const App&); // forbid copy
	App&            operator=       (const App&); // forbid assignment

private:
	Window			window_;
	CommonControls	common_ctrl_;

	bool			shading_toggle_;
	bool			shading_mode_changed_;
	bool			system_changed_;
	bool			fireBullet = false;

	glGeneratedIndices	gl_;

	float				camera_rotation_angle_;
	Timer				timer_;

	ParticleSystemType	ps_type_;
	IntegratorType		integrator_, previous_integrator_;
	ParticleSystem*		ps_;
	
	float			step_, previous_step_;
	int				steps_per_update_;
	SimpleSystem	simple_system_;
	SpringSystem	spring_system_;
	PendulumSystem	pendulum_system_;
	ClothSystem		cloth_system_;

	bool			initial_implicit_;

#ifdef COMPUTE_CLOTH_MODULE
	ComputeCloth	compute_cloth_;
#endif

#ifdef EIGEN_SPARSECORE_MODULE_H
	SparseMatrix ps_J_;
	SparseLU implicit_solver_;
#endif
};

} // namespace FW
