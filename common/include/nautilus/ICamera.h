#pragma once
#ifndef ICAMERA_H
#define ICAMERA_H

#include"Base.h"

namespace nautilus {

	namespace graphics {


		class ICamera2D {
		public:

			virtual void move(glm::vec2) = 0; // Move in some direction.
			virtual void move(float, float) = 0; // Move in some direction.
			virtual void elevate(float) = 0; // Move up, means increase camera height.
			virtual void sink(float) = 0; // Move down, means decrease camera height.


			virtual void teleport(glm::vec2) = 0; // Move instant to given position.
			virtual void teleport(float, float) = 0; // Move instant to given position.
			virtual void elevateTeleport(float) = 0; // .. same.


			virtual glm::vec2 getViewport() const = 0;


			virtual glm::mat4 getViewProjection() const = 0;

		protected:



		protected:

		};


	}


}

#endif