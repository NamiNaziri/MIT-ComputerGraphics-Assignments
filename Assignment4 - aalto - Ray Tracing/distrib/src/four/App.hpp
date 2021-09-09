#pragma once

#include "gui/Window.hpp"
#include "gui/Image.hpp"
#include "gui/CommonControls.hpp"
#include "3d/Mesh.cpp"

#include "SceneParser.h"
#include <vector>
#include <memory>

#include "raytracer.hpp"

#include "args.hpp"

namespace FW {

class App : public Window::Listener
{
private:
	enum ProjectionType {
		ORTHO_CAMERA,
		PERSPECTIVE_CAMERA
	};
	enum SamplerType {
		UNIFORM_SAMPLER = 0,
		REGULAR_SAMPLER = 1,
		JITTERED_SAMPLER = 2
	};
public:
					App             (void);
	virtual bool    handleEvent     (const Window::Event& ev);

private:
	void			initRendering		(void);
	void			render				(void);

	Mat4f			getCamera(void);
	void			copyCamera(void);

private:
					App             (const App&); // forbid copy
	App&            operator=       (const App&); // forbid assignment

	Args			get_args();

private:
	Window			window_;
	CommonControls	common_ctrl_;

	ProjectionType	camera_type_;
	Vec3f			camera_position_;
	Vec3f			camera_velocity_; // in the local coordinates of the camera
	Vec2f			camera_rotation_;
	Mat3f			scene_camera_rotation_;

	bool			mouse_right_down_;

	float			fov_, ortho_size_, camera_speed_;

	std::unique_ptr<SceneParser>	scene_;
	bool			load_scene_, raytrace_, display_results_, display_uv_;
	bool			shadows_, transparent_shadows_, shade_back_;
	SamplerType		sampler_type_;
	int				sample_count_, downscale_factor_, bounces_;

	GLuint			result_;

	std::vector<RaySegment> debug_rays;
};

} // namespace FW
