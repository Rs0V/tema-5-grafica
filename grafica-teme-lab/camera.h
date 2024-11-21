#pragma once

#include "datatypes.hpp"

///////////////////////////////////////////////////////////////////////////

struct Camera {
	// VARIABLES
	f3 origin{ 0.0f, 0.0f, 0.0f };
	f3 location{ 0.0f, 0.0f, 0.0f };
	f3 rotation{ 0.0f, 0.0f, 0.0f };

	f3 forward{ 0.0f, 0.0f, -1.0f };
	f3 right{ 1.0f, 0.0f, 0.0f };
	f3 up{ 0.0f, 1.0f, 0.0f };

	f32 xmin, xmax;
	f32 ymin, ymax;
	f32 znear, zfar;
	f32 fov;

	f32 width, height;



	// FUNCTIONS
	Camera() {
		this->xmin = -200.0f;
		this->xmax =  200.0f;

		this->ymin = -150.0f;
		this->ymax =  150.0f;

		this->znear = 0.1f;
		this->zfar  = 10000.0f;

		this->width  = 800.0f;
		this->height = 600.0f;

		this->fov = glm::radians(90.0f);
	}
	Camera(
		f32 xmin_,
		f32 xmax_,
		f32 ymin_,
		f32 ymax_,
		f32 znear_,
		f32 zfar_,
		maybe<f32> width_,
		maybe<f32> height_,
		f32 fov_degrees
	) {
		defaults(width_,  xmax_ - xmin_);
		defaults(height_, ymax_ - ymin_);

		this->xmin = xmin_;
		this->xmax = xmax_;

		this->ymin = ymin_;
		this->ymax = ymax_;

		this->znear = znear_;
		this->zfar  = zfar_;

		this->width  = width_.value();
		this->height = height_.value();

		this->fov = glm::radians(fov_degrees);
	}

	void transform(maybe<f3> location, maybe<f3> rotation) {
		defaults(location, this->location);
		defaults(rotation, this->rotation);

		this->location = location.value();
		this->rotation = rotation.value();



		f32 x_rot = glm::radians(rotation.value().x);
		f32 y_rot = glm::radians(rotation.value().y);
		f32 z_rot = glm::radians(rotation.value().z);

		m4 m4_rot = m4(1.0f)
			* glm::rotate(m4(1.0f), x_rot, f3(1.0f, 0.0f, 0.0f))
			* glm::rotate(m4(1.0f), y_rot, f3(0.0f, 1.0f, 0.0f))
			* glm::rotate(m4(1.0f), z_rot, f3(0.0f, 0.0f, 1.0f))
		;

		this->forward = f3(m4_rot * f4(0.0f, 0.0f, -1.0f, 0.0f));
		this->right = f3(m4_rot * f4(1.0f, 0.0f, 0.0f, 0.0f));
		this->up = f3(m4_rot * f4(0.0f, 1.0f, 0.0f, 0.0f));
	}
	void transform_delta(maybe<f3> delta_location, maybe<f3> delta_rotation, bool local = true) {
		defaults(delta_location, f3(0.0f));
		defaults(delta_rotation, f3(0.0f));

		f3 loc;
		if (local == true) {
			loc = this->location
				+ this->right * delta_location.value().x
				+ this->up * delta_location.value().y
				+ this->forward * delta_location.value().z
			;
		}
		else {
			loc = this->location + delta_location.value();
		}
		f3 rot = this->rotation + delta_rotation.value();

		this->location = loc;
		this->rotation = rot;



		f32 x_rot = glm::radians(rot.x);
		f32 y_rot = glm::radians(rot.y);
		f32 z_rot = glm::radians(rot.z);

		if (local == true) {
			q4 camera_orientation = q4(1.0f, 0.0f, 0.0f, 0.0f);
			camera_orientation = q4_make(x_rot, f3(1.0f, 0.0f, 0.0f)) * camera_orientation;
			camera_orientation = q4_make(y_rot, f3(0.0f, 1.0f, 0.0f)) * camera_orientation;
			camera_orientation = q4_make(z_rot, f3(0.0f, 0.0f, -1.0f)) * camera_orientation;

			this->right	  = camera_orientation * f3(1.0f, 0.0f, 0.0f);
			this->up	  = camera_orientation * f3(0.0f, 1.0f, 0.0f);
			this->forward = camera_orientation * f3(0.0f, 0.0f, -1.0f);
		}
		else {
			m4 m4_rot = m4(1.0f)
				* glm::rotate(m4(1.0f), x_rot, f3(1.0f, 0.0f, 0.0f))
				* glm::rotate(m4(1.0f), y_rot, f3(0.0f, 1.0f, 0.0f))
				* glm::rotate(m4(1.0f), z_rot, f3(0.0f, 0.0f, 1.0f))
			;

			this->right   = m3(m4_rot) * f3(1.0f, 0.0f, 0.0f);
			this->up	  = m3(m4_rot) * f3(0.0f, 1.0f, 0.0f);
			this->forward = m3(m4_rot) * f3(0.0f, 0.0f, -1.0f);
		} 
	}

	void set_origin(f3 origin) {
		this->origin = origin;
	}

	void set_fov(f32 degrees) {
		this->fov = glm::radians(degrees);
	}

	f32 get_fov() const {
		return glm::degrees(this->fov);
	}
};
