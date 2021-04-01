#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"
#include "MatrixStack.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;


shared_ptr<Camera> camera;



int main(int argc, char **argv)
{
	if(argc < 4) {
		std::cout << "Usage: SCENE | WIDTH | HEIGHT | FILENAME " << endl;
		return 0;
	}
	vector<Scene* > scenes;

	//parse command line arguments
	int scene = atoi(argv[1]);
	int width = atoi(argv[2]);
	int height(atoi(argv[3]));
	string outName(argv[4]);
	//make scene 0
		vector<Light* > lights0;
		vector<Shape* > shapes0;
		scenes.push_back(new Scene(shapes0, lights0));

	//make scene 1
		//3 spheres
		//1 light
		vector<Light* > lights1;
		vector<Shape* > shapes1;

		lights1.push_back(new Light(glm::vec3(-2.0, 1.0, 1.0), 1.0f));

		//red sphere
		shapes1.push_back(new Sphere(glm::vec3(-.5, -1.0, 1.0), 1, glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, .5), 100));
		//green sphere
		shapes1.push_back(new Sphere(glm::vec3(.5, -1.0, -1.0), 1, glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0, 1.0, .5), 100));
		//blue sphere
		shapes1.push_back(new Sphere(glm::vec3(0.0, 1.0, 0.0), 1, glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, .5), 100));
		scenes.push_back(new Scene(shapes1, lights1));
	//make scene 2
		vector<Light* > lights2;
		vector<Shape* > shapes2;
		scenes.push_back(new Scene(shapes2, lights2));
	//make scene 3
		vector<Light* > lights3;
		vector<Shape* > shapes3;

		lights3.push_back(new Light(glm::vec3(-1.0, 2.0, 1.0), 0.5f));
		lights3.push_back(new Light(glm::vec3(0.5, 0.5, .0), 0.5f));

		shapes3.push_back((new Sphere(glm::vec3(.5, -.7, 0.5), 1, glm::vec3(.3, .3, .3), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 1.0, .5), 100)));
		shapes3.push_back((new Sphere(glm::vec3(1.0, -.7, 0.5), 1, glm::vec3(.3, .3, .3), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, .5), 100)));

		shapes3.push_back((new Sphere(glm::vec3(-.5, 0, -0.5), 1, glm::vec3(1, 1, 1), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, .5), 100)));
		shapes3.back()->reflective = true;
		shapes3.push_back((new Sphere(glm::vec3(1.5, 0, -1.5), 1, glm::vec3(1, 1, 1), glm::vec3(0.0, 0.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, .5), 100)));
		shapes3.back()->reflective = true;

		shapes3.push_back(new Plane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(.1, .1, .1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), 0.0));
		shapes3.push_back(new Plane(glm::vec3(0.0, 0.0, -3.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(.1, .1, .1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), 0.0));

		scenes.push_back(new Scene(shapes3, lights3));

		//make scene 4
		vector<Light* > lights4;
		vector<Shape* > shapes4;
		scenes.push_back(new Scene(shapes4, lights4));

		//make scene 5
		vector<Light* > lights5;
		vector<Shape* > shapes5;
		scenes.push_back(new Scene(shapes2, lights5));

	auto image = make_shared<Image>(width, height);

	camera = make_shared<Camera>();
	camera->setInitDistance(5.0f);

	//initialize zbuffer to 0
	vector<vector<float> > zbuffer;
	for (int i = 0; i < width+1; i++) {
		vector<float> temp;
		for (int j = 0; j < height+1; j++) {
			temp.push_back(INT_MIN);
		}
		zbuffer.push_back(temp);
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			glm::vec3 ray[2];
			//y = height - y;
			//shoot ray from camera to screen position
			auto M = make_shared<MatrixStack>();
			M->loadIdentity();
			camera->applyProjectionMatrix(M);
			glm::mat4 P = M->topMatrix();
			M->loadIdentity();
			camera->applyViewMatrix(M);
			
			glm::mat4 V = M->topMatrix();
			glm::mat4 C = inverse(V);
			glm::mat4 Pinv = inverse(P);


			//find ray direction in world coords
			ray[0] = C[3];


			//pixel coords -> normalized device coords
			float pn[2];
			pn[0] = ((2.0 * x) / width) - 1.0;
			pn[1] = 1.0 - ((2.0 * y) / height);

			//normalized device coords -> clip coords
			glm::vec4 pc = glm::vec4(pn[0], pn[1], -1.0, 1.0);

			//clip coords -> eye coords
			glm::vec4 pe = Pinv * pc;

			//manually set w to 1.0
			pe[3] = 1.0f;

			//eye coords ->w world coords
			glm::vec4 pw = C * pe;
			glm::vec3 vw = glm::vec3(pw) - ray[0];

			vw = glm::normalize(vw);
			ray[1] = vw;

			//we should now have a ray from camera position through the x, y coord

			//we now check all the objects in the chosen scene for collisions

			for (int i = 0; i < scenes[scene]->shapes.size(); i++) {
			//	//translate shape values to world coords
				bool h;
				float t;
				scenes[scene]->shapes[i]->intersect(ray[0], ray[1], h, t);
				if (h) {
					glm::vec3 hitpoint = (ray[0] + t * ray[1]);
					glm::vec3 color = scenes[scene]->shapes[i]->CalculateColor(lights1, scenes[scene]->shapes, scenes[scene]->shapes[i], hitpoint, ray[0]);
					for (int k = 0; k < 3; k++) {
						if (color[k] > 1) {
							color[k] = 1;
						}
					}
					//if the pixel is the shallowest seen yet
					if (hitpoint[2] > zbuffer.at(x).at(y)) {

						//update zbuffer with depth value
						zbuffer.at(x).at(y) = hitpoint[2];
						image->setPixel(x, height-y, color[0] * 255.0, color[1] * 255.0, color[2] * 255.0);
					}
				}
			}

		}
	}


	image->writeToFile(outName);

	

	//write framebuffer to output
	//image->writeToFile(outName);

	return 0;
}
