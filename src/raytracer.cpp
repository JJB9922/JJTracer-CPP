#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "include/raytracer.h"

static constexpr int imageHeight = 400;
static constexpr int imageWidth = 400;

float calculateDistance(const Point &p1, const Point &p2)
{
    float dx = p2.position.x - p1.position.x;
    float dy = p2.position.y - p1.position.y;
    float dz = p2.position.z - p1.position.z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

bool intersectSphere(const Ray &ray, const Sphere &sphere, Point &intersectionPoint, Normal &normal)
{
    Vector3 oc = ray.origin.position - sphere.center;

    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - (sphere.radius * sphere.radius);

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return false;

    float sqrtDiscriminant = sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

    if (t1 >= 0 || t2 >= 0)
    {
        float t = (t1 < t2) ? t1 : t2;
        intersectionPoint.position.x = ray.origin.position.x + ray.direction.x * t;
        intersectionPoint.position.y = ray.origin.position.y + ray.direction.y * t;
        intersectionPoint.position.z = ray.origin.position.z + ray.direction.z * t;
        normal.direction = (intersectionPoint.position - sphere.center).normalized();
        return true;
    }

    return false;
}

Colour traceRay(const Ray &ray, const std::vector<Object> &objects, const std::vector<Light> &lights)
{
    Colour pixelColour = {0, 0, 0, 255}; //Bakgrounbd

    for (const auto &object : objects)
    {
        Point intersectionPoint;
        Normal normal;
        if (object.shape == SPHERE && intersectSphere(ray, object.geometry.sphere, intersectionPoint, normal))
        {
            Colour objectColour = object.colour;
            for (const auto &light : lights)
            {
                Vector3 toLight = light.position - intersectionPoint.position;
                Ray shadowRay = {intersectionPoint, toLight.normalized()};
                bool isInShadow = false;
                for (const auto &otherObject : objects)
                {
                    if (otherObject.shape == SPHERE && intersectSphere(shadowRay, otherObject.geometry.sphere, intersectionPoint, normal))
                    {
                        isInShadow = true;
                        break;
                    }
                }
                if (!isInShadow)
                {
                    float diffuseFactor = normal.direction.dot(toLight.normalized());
                    if (diffuseFactor > 0)
                    {
                        pixelColour.r += objectColour.r * light.colour.r * diffuseFactor * light.brightness;
                        pixelColour.g += objectColour.g * light.colour.g * diffuseFactor * light.brightness;
                        pixelColour.b += objectColour.b * light.colour.b * diffuseFactor * light.brightness;
                    }
                }
            }
        }
    }

    return pixelColour;
}

int main()
{
    std::cout << "Build Success!" << std::endl;

    Light light = {{0.0f, 0.0f, 10.0f}, {255, 255, 255, 255}, 1.0f};

    Sphere mainSphere = {{0.0f, 0.0f, 0.0f}, 10.0f};
    Object mainSphereObject = {{255, 255, 255, 255}, SPHERE, {mainSphere}};

    Sphere sphere1 = {{0.0f, 10.0f, 30.0f}, 5.0f};
    Object sphereObject1 = {{0, 255, 0, 255}, SPHERE, {sphere1}};

    Sphere sphere2 = {{-20.0f, 20.0f, 20.0f}, 7.0f};
    Object sphereObject2 = {{0, 0, 255, 255}, SPHERE, {sphere2}};

    Sphere sphere3 = {{20.0f, -20.0f, 5.0f}, 8.0f};
    Object sphereObject3 = {{255, 0, 0, 255}, SPHERE, {sphere3}};

    Sphere sphere4 = {{-20.0f, -20.0f, -5.0f}, 6.0f};
    Object sphereObject4 = {{255, 255, 0, 255}, SPHERE, {sphere4}};

    float lightDistance = 2.5f * mainSphere.radius;
    light.position.z = sphere2.center.z + lightDistance;

    float orbitRadius = 25.0f;
    float orbitSpeed = 0.1f;
    Vector3 orbitCenter = mainSphereObject.geometry.sphere.center;
    float angle = 0.0f;

    std::vector<Object> objects = {mainSphereObject, sphereObject1, sphereObject2, sphereObject3, sphereObject4};

    std::vector<Light> lights = {light};

    SDL_Window *window = SDL_CreateWindow("Ray Tracing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, imageWidth, imageHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    int cycleSpeed = 1;
    int cycleCounter = 0;

    bool quit = false;
    SDL_Event e;
    while (!quit)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
            light.colour.r = (255 + cos(cycleCounter * cycleSpeed)) / 2;
            light.colour.g = (255 + coshf32(cycleCounter * cycleSpeed + 2)) / 16;
            light.colour.b = (255 + sin(cycleCounter * cycleSpeed + 4)) / 2;
        cycleCounter++;

        for (int i = 0; i < imageHeight; ++i)
        {
            for (int j = 0; j < imageWidth; ++j)
            {
                Ray ray;
                ray.origin.position = {0.0f, 0.0f, 50.0f};
                ray.direction = {(2.0f * j / imageWidth) - 1.0f, 1.0f - (2.0f * i / imageHeight), -1.0f};
                ray.direction.normalize();

                Colour pixelColour = traceRay(ray, objects, lights);

                SDL_SetRenderDrawColor(renderer, pixelColour.r, pixelColour.g, pixelColour.b, pixelColour.a);
                SDL_RenderDrawPoint(renderer, j, i);
            }
        }

        SDL_RenderPresent(renderer);

        float offsetX = orbitRadius * cos(angle);
        float offsetY = orbitRadius * sin(angle);
        light.position.x = orbitCenter.x + offsetX;
        light.position.y = orbitCenter.y + offsetY;

        objects.clear();
        lights.clear();
        objects.push_back(mainSphereObject);
        objects.push_back(sphereObject1);
        objects.push_back(sphereObject2);
        objects.push_back(sphereObject3);
        objects.push_back(sphereObject4);
        lights.push_back(light);

        angle += orbitSpeed;

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                quit = true;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}