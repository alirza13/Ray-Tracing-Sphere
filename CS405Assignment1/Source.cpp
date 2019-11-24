#include <list> 
#include <math.h>
#include <limits>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

class Shape {   // For future implementations.


public:
	Shape();



};

Shape::Shape() {

}

// Vector Class
class Vector
{
public:
	Vector();
	Vector(double x, double y, double z);
	Vector operator - (Vector direction);
	Vector operator + (Vector direction);
	Vector operator * (double d) { return Vector(x*d, y*d, z*d); }
	Vector operator / (double d) { return Vector(x*d, y*d, z*d); }
	Vector normalize() const {   // Normalize vector.
		double mg = sqrt(x*x + y * y + z * z);
		return Vector(x / mg, y / mg, z / mg);
	}
	~Vector();
public:
	double x, y, z;
};

Vector::Vector()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector::Vector(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector Vector::operator- (Vector direction) {
	return  Vector(x - direction.x, y - direction.y, z - direction.z);
}

Vector Vector::operator+ (Vector direction) {
	return  Vector(x + direction.x, y + direction.y, z + direction.z);
}


Vector::~Vector()
{
}

double dotProduct(Vector first, Vector second)
{
	return (first.x * second.x + first.y * second.y + first.z * second.z);
}

// Vector Class

class Ray {
public:
	Vector origin, direction;

public:
	Ray();
	Ray(Vector o, Vector d);

};

Ray::Ray(Vector o, Vector d) {
	this->origin = o;
	this->direction = d;
}

class Sphere : Shape {

public:
	Sphere();
	Sphere(Vector center, double radius);
	bool isIntersected(Ray ray, double &t);
	Vector getNormal(Vector P) const { return (P - center) / radius; }
	Sphere operator + (Sphere other);
	Vector center;
	double radius;

};

Sphere::Sphere(Vector center, double radius) {
	this->center = center;
	this->radius = radius;
}

Sphere Sphere::operator+ (Sphere other) {
	return Sphere(other.center, other.radius);
}


bool Sphere::isIntersected(Ray ray, double &t) {

	Vector difference = center - ray.origin;
	double tca = dotProduct(difference, ray.direction);
	if (tca < 0) return false;
	double squareDifference = dotProduct(difference, difference);
	double d = sqrt(squareDifference - tca * tca);
	if (d < 0) return false;
	if (radius * radius - d * d > 0)
	{
		double thc = sqrt(radius * radius - d * d);
		double t1 = tca - thc;
		double t2 = tca + thc;

		if (t1 > t2)
			t = t2;
		else
			t = t1;

	}

	else
		return false;

	return true;

}


class Triangle : public Shape {
public:
	Triangle();
};

Vector Restrict(Vector color) {
	if (color.x > 255)
		color.x = 255;
	else if (color.x < 0)
		color.x = 0;
	if (color.y > 255)
		color.y = 255;
	else if (color.y < 0)
		color.y = 0;
	if (color.z > 255)
		color.z = 255;
	else if (color.z < 0)
		color.z = 0;

	return color;
}

int main()
{
	string line;
	ifstream myfile("configuration.txt");
	int resolutionH = 3840;
	int resolutionW = 2160;
	vector<string> result;
	double spherex, spherey, spherez, sphereRadius;
	double lightx, lighty, lightz, lightRadius;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			istringstream stream(line);
			for (string s; stream >> s; )
				result.push_back(s);
			if (result[0] == "Height") {
				resolutionH = stoi(result[1]);
			}
			else if (result[0] == "Weight") {
				resolutionW = stoi(result[1]);
			}
			else if (result[0] == "Sphere")
			{
				spherex = stoi(result[1]);
				spherey = stoi(result[2]);
				spherez = stoi(result[3]);
				sphereRadius = stoi(result[4]);
			}
			else if (result[0] == "Light")
			{
				lightx = stoi(result[1]);
				lighty = stoi(result[2]);
				lightz = stoi(result[3]);
				lightRadius = stoi(result[4]);
			}
			result.clear();
		}
		myfile.close();
	}

	vector <Sphere> vectorShape;
	double t = 20000;

	Vector white(255, 255, 255);
	Vector black(0, 0, 0);
	Vector red(255,0,0);

	Sphere sphere(Vector(spherex, spherey, spherez), sphereRadius);
	Sphere sphere1(Vector(300, 300, 300), 100);
	Sphere light(Vector(lightx, lighty, lightz), lightRadius);

	vectorShape.push_back(sphere);
	vectorShape.push_back(sphere1);
	Vector pixelColor(black);
	Vector P;
	Vector LT;
	Vector normal;
	double product;

	ofstream out("./my_scene0001.ppm", ios::binary);       // PPM file format.
	out << "P3\n" << resolutionW << " " << resolutionH << "\n255\n";
	Sphere selectedSphere(Vector(0, 0, 0), 0);
	bool intersect = false;
	int Min;
	for (int i = 0; i < resolutionH; i++)
	{
		for (int j = 0; j < resolutionW; j++)
		{
			pixelColor = black;
			Ray ray(Vector(i, j, 0), Vector(0, 0, 1));

			Min = INT_MAX;
			for (int k = 0; k < vectorShape.size(); k++) {
				intersect = sphere.isIntersected(ray, t);
				if (intersect)
				{
					if (t > 0 && t < Min) {
						Min = t;
						selectedSphere = vectorShape[k];
					}
				}
			}
			
			if (intersect)
			{
				P = ray.origin + ray.direction * Min;  // Ray equation
				LT = light.center - P;  // Light equation
				normal = selectedSphere.getNormal(P);
				product = dotProduct(LT.normalize(), normal.normalize());
				pixelColor = (red + white * product) * 0.5;
				pixelColor = Restrict(pixelColor);
			}

			out << (int)pixelColor.x << " "
				<< (int)pixelColor.y << " "
				<< (int)pixelColor.z << "\n";
		}
	}

	out.close();
	return 0;
}


