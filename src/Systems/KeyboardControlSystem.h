#ifndef KEYBOARDSYSTEM_H
#define KEYBOARDSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include <SDL.h>

class KeyboardControlSystem : public System {
public:
	KeyboardControlSystem() {
		RequireComponent<KeyboardControlledComponent>();
		RequireComponent<SpriteComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		for (auto entity : GetSystemEntities()) {
			const auto& keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();
			auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			switch (event.symbol) {
				case SDLK_UP:
					rigidbody.velocity = keyboardControl.upVelocity;
					sprite.srcRect.y = sprite.height * 0;
					break;
				case SDLK_RIGHT:
					rigidbody.velocity = keyboardControl.rightVelocity;
					sprite.srcRect.y = sprite.height * 1;
					break;
				case SDLK_DOWN:
					rigidbody.velocity = keyboardControl.downVelocity;
					sprite.srcRect.y = sprite.height * 2;
					break;
				case SDLK_LEFT:
					rigidbody.velocity = keyboardControl.leftVelocity;
					sprite.srcRect.y = sprite.height * 3;
					break;
			}
		}
	}

	void Update(std::unique_ptr<EventBus>& eventBus) {

	}


};

#endif // !KEYBOARDSYSTEM_H