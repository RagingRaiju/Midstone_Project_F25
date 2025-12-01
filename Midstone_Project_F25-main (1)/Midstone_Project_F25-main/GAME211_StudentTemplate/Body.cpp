#include "Body.h"

Body::Body()
{
    pos = Vec3();
    vel = Vec3();
    accel = Vec3();
	mass = 1.0f;
    radius = 0.0f;
    orientation = 0.0f;
    rotation = 0.0f;
    angular = 0.0f;

	image = nullptr;
}

Body::Body(
    Vec3 pos_, Vec3 vel_, Vec3 accel_,
    float mass_,
    float radius_ = 0.0f,
    float orientation_ = 0.0f,
    float rotation_ = 0.0f,
    float angular_ = 0.0f
)
{
    pos = pos_;
    vel = vel_;
    accel = accel_;
    mass = mass_;
    radius = radius_;
    orientation = orientation_;
    rotation = rotation_;
    angular = angular_;

    image = nullptr;
}


Body::~Body(){
    if (image != NULL)
    {
        SDL_DestroySurface(image);
    }
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
    }
}

void Body::ApplyForce( Vec3 force_ ) {
    accel = force_ / mass;
}

void Body::Update( float deltaTime ){
    pos = pos + vel * deltaTime + accel * (0.5f * deltaTime * deltaTime);
    vel = vel + accel * deltaTime;
    // Update orientation
    orientation += rotation * deltaTime;
    rotation += angular * deltaTime;

}

void Body::HandleEvents( const SDL_Event& event )
{
    if ( event.type == SDL_EVENT_MOUSE_BUTTON_DOWN )
    {
        printf("Mousedown\n");
    }
    else if ( event.type == SDL_EVENT_KEY_DOWN )
    {
        if ( event.key.scancode == SDL_SCANCODE_SPACE)
        {
            printf("Space\n");
        }
    }
    // etc
}

void Body::setPos( Vec3 pos_ )
{
    pos = pos_;
}
