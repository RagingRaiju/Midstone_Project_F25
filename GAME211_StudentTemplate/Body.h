#ifndef BODY_H
#define BODY_H

#define SDL_ENABLE_OLD_NAMES

#include <MMath.h>
#include <VMath.h>
#include <SDL.h>
#include <SDL_image.h>

using namespace MATH;

class Body {
protected:
	// inherited classes can access this
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	float mass;
    float orientation;		// facing this direction
    float rotation;			// rotating at this speed (radians per second?)
    float angular;          // angular acceleration
    float radius;           // for getting near walls

	Vec3 imageSizeWorldCoords;
    SDL_Surface* image;
    SDL_Texture* texture;

public:
    Body();
	Body(
        Vec3 pos_, Vec3 vel_, Vec3 accel_,
        float mass_,
        float radius_,
        float orientation_,
        float rotation_,
        float angular_
    );
	virtual ~Body();
    virtual void Update(float deltaTime);
    virtual void ApplyForce(Vec3 force_);
    virtual Vec3 getPos() const { return pos; }
    virtual Vec3 getVel() const { return vel; }
    virtual Vec3 getAccel() const { return accel; }
    virtual float getMass() const { return mass; }
    virtual float getOrientation() const { return orientation; }
    virtual float getRotation() const { return rotation; }
    virtual float getAngular() const { return angular; }
    virtual float getRadius() const { return radius; }

    virtual void setImage(SDL_Surface* image_) 
    {
        if (image != NULL)
        {
            SDL_DestroySurface(image);
        }
        image = image_;
    }
    virtual SDL_Surface* getImage() const { return image; }

    virtual void setImageSizeWorldCoords(Vec3 imageSizeWorldCoords_)
    {
        imageSizeWorldCoords = imageSizeWorldCoords_;
	}

    virtual void setTexture( SDL_Texture* texture_ ) 
    {
        if (texture != NULL)
        {
            SDL_DestroyTexture(texture);
        }
        texture = texture_;
    }
    virtual SDL_Texture* getTexture() const { return texture; }

    virtual void HandleEvents( const SDL_Event& event );

	// Added this as public to deal with my demo
	// so that mouse position can be copied into a Body.
    virtual void setPos( Vec3 pos );
};

#endif /* BODY_H */
