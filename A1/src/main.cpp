#include <iostream>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

struct vertex {
	float x = 0;
	float y = 0;
	float z = 0;
	float r = 0;
	float g = 0;
	float b = 0;
	
	vertex() {
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;
	}

	vertex(float a, float _b, float c) {
		x = a;
		y = _b;
		z = c;
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;
	}
};

float area(vertex  v0, vertex v1, vertex v2) {
	return ( .5 * ((v1.x - v0.x) * (v2.y - v0.y)-(v2.x-v0.x)*(v1.y-v0.y)));
}


struct triangle {
	vertex verticies[3];

	//initialize bounding box values
	float xmin = INT_MAX; float ymin = INT_MAX; float zmin;
	float xmax = INT_MIN; float ymax = INT_MIN; float zmax;

	triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
		verticies[0] = vertex(x1 , y1, z1);
		verticies[1] = vertex(x2 , y2 , z2);
		verticies[2] = vertex(x3 , y3 , z3);
		
		//create triangles bounding box
		if (x1 > xmax) {xmax = x1;}
		if (x1 < xmin) {xmin = x1;}
		if (x2 > xmax) {xmax = x2;}
		if (x2 < xmin) {xmin = x2;}
		if (x3 > xmax) {xmax = x3;}
		if (x3 < xmin) {xmin = x3;}

		if (y1 > ymax) {ymax = y1;}
		if (y1 < ymin) {ymin = y1;}
		if (y2 > ymax) {ymax = y2;}
		if (y2 < ymin) {ymin = y2;}
		if (y3 > ymax) {ymax = y3;}
		if (y3 < ymin) {ymin = y3;}

		if (z1 > zmax) { zmax = z1; }
		if (z1 < zmin) { zmin = z1; }
		if (z2 > zmax) { zmax = z2; }
		if (z2 < zmin) { zmin = z2; }
		if (z3 > zmax) { zmax = z3; }
		if (z3 < zmin) { zmin = z3; }
	}


	//calculate whether point is inside triangle
	bool inside(float x, float y) {
		vertex point(x, y, 0);

		//calculate barycentric values
		float totalarea = area(verticies[0], verticies[1], verticies[2]);
		float a = area(point, verticies[1], verticies[2])/totalarea;
		float b = area(point, verticies[2], verticies[0])/totalarea;
		float c = area(point, verticies[0], verticies[1])/totalarea;

		//logic determines if the point lies within the triangle
		if ((a > 0) && ( b > 0) && (c > 0)) {
			return true;
		}
		return false;
	}

	//interpolate specific depth based on vertex z values
	float baryZ(float x,float y) {
		vertex point(x, y, 0);

		float totalarea = area(verticies[0], verticies[1], verticies[2]);
		float a = area(point, verticies[0], verticies[1]) / totalarea;
		float b = area(point, verticies[1], verticies[2]) / totalarea;
		float c = area(point, verticies[2], verticies[0]) / totalarea;

		return ( a * verticies[2].z + b * verticies[0].z + c * verticies[1].z);
	}

	//return a vertex with .r .g .b according to barycentric interpolation of verticies
	vertex color(float x, float y) {
		vertex point(x, y, 0);

		float totalarea = area(verticies[0], verticies[1], verticies[2]);
		float a = area(point, verticies[0], verticies[1]) / totalarea;
		float b = area(point, verticies[1], verticies[2]) / totalarea;
		float c = area(point, verticies[2], verticies[0]) / totalarea;

		//scale colors and return
		point.r = a * verticies[2].r + b * verticies[0].r + c * verticies[1].r;
		point.g = a * verticies[2].g + b * verticies[0].g + c * verticies[1].g;
		point.b = a * verticies[2].b + b * verticies[0].b + c * verticies[1].b;

		return point;

	}

	//print vertex positions and min/max
	void printVerts() {
		std::cout << "( " << verticies[0].x << " , " << verticies[0].y << " , " << verticies[0].z << ")" << "\t";
		std::cout << "( " << verticies[1].x << " , " << verticies[1].y << " , " << verticies[1].z << ")" << "\t";
		std::cout << "( " << verticies[2].x << " , " << verticies[2].y << " , " << verticies[2].z << ")" << endl;
		std::cout << "xmin: " << xmin << " xmax: " << xmax << " ymin: " << ymin << " ymax: " << ymax << endl;

	}

};

int main(int argc, char **argv)
{
	if(argc < 5) {
		std::cout << "Usage: meshName | outputName | width | heigth | colormode" << endl;
		return 0;
	}

	//parse command line arguments
	string meshName(argv[1]);
	string outName(argv[2]);
	int width(atoi(argv[3]));
	int height(atoi(argv[4]));
	int mode(atoi(argv[5]));

	// Load geometry
	vector<float> posBuf; // list of vertex positions
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	vector<triangle> triangles;

	float meshxmax = INT_MIN;
	float meshymax = INT_MIN;
	float meshxmin = INT_MAX;
	float meshymin = INT_MAX;
	float meshzmin = INT_MAX;
	float meshzmax = INT_MIN;

	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					if ((attrib.vertices[3 * idx.vertex_index + 0]) > meshxmax) {
						meshxmax = attrib.vertices[3 * idx.vertex_index + 0];
					}
					if ((attrib.vertices[3 * idx.vertex_index + 0]) < meshxmin) {
						meshxmin = attrib.vertices[3 * idx.vertex_index + 0];
					}

					posBuf.push_back(attrib.vertices[3 *idx.vertex_index+1]);
					if (((attrib.vertices[3.0 * idx.vertex_index + 1]) > meshymax)) {
						meshymax = (attrib.vertices[3 * idx.vertex_index + 1]);
					}
					if (((attrib.vertices[3.0 * idx.vertex_index + 1])) < meshymin) {
						meshymin = (attrib.vertices[3 * idx.vertex_index + 1]);
					}

					posBuf.push_back(attrib.vertices[3 * idx.vertex_index+2]);
					if (((attrib.vertices[3.0 * idx.vertex_index + 1]) > meshzmax)) {
						meshzmax = (attrib.vertices[3 * idx.vertex_index + 1]);
					}
					if (((attrib.vertices[3.0 * idx.vertex_index + 1])) < meshzmin) {
						meshzmin = (attrib.vertices[3 * idx.vertex_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}

	shapes.clear();

	//calculate scaling and offset
	float xscale = width / (meshxmax - meshxmin);
	float yscale = height / (meshymax - meshymin);
	float scale, min;

	//decide which bound to max out
	if (xscale > yscale) {
		scale = yscale;
		min = meshymin;	
	}
	else {
		scale = xscale;
		min = meshxmin;
	}

	//calculate scaled mesh bounds
	meshxmin = scale * (meshxmin - min);
	meshxmax = scale * (meshxmax - min);
	meshymin = scale * (meshymin - min);
	meshymax = scale * (meshymax - min);

	//find translation values
	float meshxmidpoint = meshxmax - ((meshxmax - meshxmin) / 2);
	float meshymidpoint = meshymax - ((meshymax - meshymin) / 2);
	float xoffset = width / 2 - meshxmidpoint;
	float yoffset = height / 2 - meshymidpoint;


	//build triangles
	//constructor gives vertices random colors
	for (int i = 0; i < posBuf.size(); i += 9) {
		triangle newtri(scale*(posBuf[i]-min) + xoffset, scale*(posBuf[i+1]-min) + yoffset, posBuf[i+2], 
						scale*(posBuf[i+3]-min)+xoffset, scale*(posBuf[i+4]-min) + yoffset, posBuf[i+5], 
						scale*(posBuf[i+6]-min)+xoffset, scale*(posBuf[i+7]-min) + yoffset, posBuf[i+8]);

		triangles.push_back(newtri);
	}

	//update mesh bounds with scaled values
	float newmeshxmax = INT_MIN;
	float newmeshymax = INT_MIN;
	float newmeshxmin = INT_MAX;
	float newmeshymin = INT_MAX;
	float newmeshzmax = INT_MIN;
	float newmeshzmin = INT_MAX;
	for (int i = 0; i < triangles.size(); i++) {
		if (triangles[i].xmax > newmeshxmax) {newmeshxmax = triangles[i].xmax;}
		if (triangles[i].xmin < newmeshxmin) {newmeshxmin = triangles[i].xmin;}
		if (triangles[i].ymax > newmeshymax) {newmeshymax = triangles[i].ymax;}
		if (triangles[i].ymin < newmeshymin) {newmeshymin = triangles[i].ymin;}
		if (triangles[i].zmax > newmeshzmax) {newmeshzmax = triangles[i].zmax;}
		if (triangles[i].zmin < newmeshzmin) {newmeshzmin = triangles[i].zmin;}
	}

	//make an image to draw on
	auto image = make_shared<Image>(width, height);


	//initialize zbuffer to 0
	vector<vector<int> > zbuffer;
	for (int i = 0; i < width; i++) {
		vector<int> temp;
		for (int j = 0; j < height; j++) {
			temp.push_back(0);
		}
		zbuffer.push_back(temp);
	}


	//walk over all triangles
	for (int i = 0; i < triangles.size(); i++) {

		//walk over all pixels in the triangles bounding box
		for (int x = triangles[i].xmin; x < triangles[i].xmax; x++) {
			for (int y = triangles[i].ymin; y < triangles[i].ymax; y++) {

				//if the pixel is inside the triangles
				if (triangles[i].inside(x, y)) {

					//interpolate color and depth
					vertex colors;

					//Z scaled from 0-255 for RGB val
					float z = 255* (triangles[i].baryZ(x, y) - newmeshzmin)/(newmeshzmax-newmeshzmin);
	
					//if the pixel is the shallowest seen yet
					if (z > zbuffer.at(x).at(y)) {

						//update zbuffer with depth value
						zbuffer.at(x).at(y) = z;

						//barycentric vertex color interpolation
						if (mode == 0) {
							colors = triangles[i].color(x, y);
						}
						//scaled zvalue depth coloring
						else if (mode == 1) {
							colors.r = z; colors.g = 0; colors.b = 0;
						}
						//scaled yvalue gradient
						else if (mode == 2) {
							float newy = 255*(y-newmeshymin) / (newmeshymax - newmeshymin);
							colors.r = 255-newy; colors.g = 0; colors.b = newy;
						}

						//update framebuffer with color value
						image->setPixel(x, y, colors.r, colors.g, colors.b);
					}
				}
			}
		}
	}

	//write framebuffer to output
	image->writeToFile(outName);

	return 0;
}
