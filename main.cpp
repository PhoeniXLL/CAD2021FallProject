#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <map>

#include <poly2tri/poly2tri.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int MAXLINELENGTH = 100;

struct Point {
	double x, y, z;
	Point(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}
	void read(char *s) {
		sscanf(s, "%lf%lf%lf", &x, &y, &z);
	}
	void print() {
		printf("%f %f %f\n", x, y, z);
	}
	double len2() {
		return x * x + y * y + z * z;
	}
	Point operator + (const Point &a) const {
		return Point(x + a.x, y + a.y, z + a.z);
	}
	Point operator - (const Point &a) const {
		return Point(x - a.x, y - a.y, z - a.z);
	}
	Point operator / (const double &a) const {
		return Point(x / a, y / a, z / a);
	}
	friend Point cross(const Point &a, const Point &b) {
		return Point(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
};

struct HalfEdge {
	int a, b;
	int oppo, last, next;
	void print() {
		printf("%d %d\n", a, b);
		printf("%d %d %d\n", oppo, last, next);
	}
};

struct Loop {
	int head;
	int face;
	void print() {
		printf("%d %d\n", head, face);
	}
};

struct Face {
	std::vector<int> loop;
	void print() {
		for (auto i : loop) {
			printf("%d ", i);
		}
		printf("\n");
	}
};

struct Solid {
	std::vector<int> face;
	void print() {
		for (auto i : face) {
			printf("%d ", i);
		}
		printf("\n");
	}
};

struct Command {
	int type;
	int a, b, c;
	Point d;
	Command(int _type = 0, int _a = -1, int _b = -1, int _c = -1, Point _d = Point(-1, -1, -1)) : type(_type), a(_a), b(_b), c(_c), d(_d) {}
};

Solid solid;
std::vector<Point> vecPoint;
std::vector<HalfEdge> vecHalfEdge;
std::vector<Loop> vecLoop;
std::vector<Face> vecFace;

void newPoint() {
	Point point;
	vecPoint.push_back(point);
}

void newHalfEdge() {
	HalfEdge halfEdge;
	vecHalfEdge.push_back(halfEdge);
}

void newLoop() {
	Loop loop;
	vecLoop.push_back(loop);
}

void newFace() {
	Face face;
	vecFace.push_back(face);
}

void mev(const int &loopLabel, const int &point1Label, const Point &point2) {
	int point2Label;
	vecPoint.push_back(point2);
	printf("Created a point labeled %d.\n", point2Label = vecPoint.size() - 1);
	
	newHalfEdge();
	newHalfEdge();
	int halfEdge1Label = vecHalfEdge.size() - 2;
	int halfEdge2Label = vecHalfEdge.size() - 1;
	printf("Created an edge labeled %d.\n", halfEdge1Label / 2);
	HalfEdge &halfEdge1 = vecHalfEdge[halfEdge1Label];
	HalfEdge &halfEdge2 = vecHalfEdge[halfEdge2Label];
	if (vecLoop[loopLabel].head == -1) {
		halfEdge1.a = point1Label;
		halfEdge1.b = point2Label;
		halfEdge1.oppo = halfEdge2Label;
		halfEdge1.last = halfEdge2Label;
		halfEdge1.next = halfEdge2Label;

		halfEdge2.a = point2Label;
		halfEdge2.b = point1Label;
		halfEdge2.oppo = halfEdge1Label;
		halfEdge2.last = halfEdge1Label;
		halfEdge2.next = halfEdge1Label;

		vecLoop[loopLabel].head = halfEdge1Label;
	} else {
		halfEdge1.a = point1Label;
		halfEdge1.b = point2Label;
		halfEdge1.oppo = halfEdge2Label;
		halfEdge1.next = halfEdge2Label;

		halfEdge2.a = point2Label;
		halfEdge2.b = point1Label;
		halfEdge2.oppo = halfEdge1Label;
		halfEdge2.last = halfEdge1Label;

		int now = vecLoop[loopLabel].head;
		while (vecHalfEdge[now].b != point1Label) {
			now = vecHalfEdge[now].next;
		}
		int lastHalfEdgeLabel = now;
		int nextHalfEdgeLabel = vecHalfEdge[now].next;
		HalfEdge &lastHalfEdge = vecHalfEdge[lastHalfEdgeLabel];
		HalfEdge &nextHalfEdge = vecHalfEdge[nextHalfEdgeLabel];

		lastHalfEdge.next = halfEdge1Label;
		halfEdge1.last = lastHalfEdgeLabel;
		nextHalfEdge.last = halfEdge2Label;
		halfEdge2.next = nextHalfEdgeLabel;
	}
}

void mef(const int &loopLabel, const int &point1Label, const int &point2Label) {
	newHalfEdge();
	newHalfEdge();
	int halfEdge1Label = vecHalfEdge.size() - 2;
	int halfEdge2Label = vecHalfEdge.size() - 1;
	printf("Created an edge labeled %d.\n", halfEdge1Label / 2);
	HalfEdge &halfEdge1 = vecHalfEdge[halfEdge1Label];
	HalfEdge &halfEdge2 = vecHalfEdge[halfEdge2Label];
	
	halfEdge1.a = point1Label;
	halfEdge1.b = point2Label;
	halfEdge1.oppo = halfEdge2Label;

	halfEdge2.a = point2Label;
	halfEdge2.b = point1Label;
	halfEdge2.oppo = halfEdge1Label;

	int now = vecLoop[loopLabel].head;
	while (vecHalfEdge[now].b != point1Label) {
		now = vecHalfEdge[now].next;
	}
	int last1HalfEdgeLabel = now;
	int next1HalfEdgeLabel = vecHalfEdge[now].next;

	now = vecLoop[loopLabel].head;
	while (vecHalfEdge[now].b != point2Label) {
		now = vecHalfEdge[now].next;
	}
	int last2HalfEdgeLabel = now;
	int next2HalfEdgeLabel = vecHalfEdge[now].next;

	HalfEdge &last1HalfEdge = vecHalfEdge[last1HalfEdgeLabel];
	HalfEdge &next1HalfEdge = vecHalfEdge[next1HalfEdgeLabel];
	HalfEdge &last2HalfEdge = vecHalfEdge[last2HalfEdgeLabel];
	HalfEdge &next2HalfEdge = vecHalfEdge[next2HalfEdgeLabel];

	vecLoop[loopLabel].head = halfEdge2Label;
	halfEdge2.last = last2HalfEdgeLabel;
	last2HalfEdge.next = halfEdge2Label;
	halfEdge2.next = next1HalfEdgeLabel;
	next1HalfEdge.last = halfEdge2Label;

	newLoop();
	printf("Created a loop labeled %u.\n", vecLoop.size() - 1);
	vecLoop.back().head = halfEdge1Label;
	halfEdge1.last = last1HalfEdgeLabel;
	last1HalfEdge.next = halfEdge1Label;
	halfEdge1.next = next2HalfEdgeLabel;
	next2HalfEdge.last = halfEdge1Label;

	newFace();
	printf("Created a face labeled %u.\n", vecFace.size() - 1);
	vecLoop.back().face = vecFace.size() - 1;
	solid.face.push_back(vecFace.size() - 1);
	vecFace.back().loop.push_back(vecLoop.size() - 1);
}

void mvfs(const Point &point) {
	newPoint();
	vecPoint.back() = point;
	printf("Created a point labeled 0.\n");

	newLoop();
	vecLoop.back().head = -1;
	vecLoop.back().face = 0;
	printf("Created a loop labeled 0.\n");
	
	newFace();
	printf("Created a face labeled 0.\n");
	vecFace[0].loop.push_back(0);
	
	solid.face.push_back(0);
}

void kemr(const int &loopLabel, const int &edgeLabel) {
	int now = vecLoop[loopLabel].head;
	while (now != edgeLabel * 2) {
		now = vecHalfEdge[now].next;
	}
	int halfEdge1Label = now;
	int halfEdge2Label = vecHalfEdge[now].oppo;
	HalfEdge &halfEdge1 = vecHalfEdge[halfEdge1Label];
	HalfEdge &halfEdge2 = vecHalfEdge[halfEdge2Label];
	
	vecLoop[loopLabel].head = halfEdge1.last;
	newLoop();
	printf("Created a loop labeled %u.\n", vecLoop.size() - 1);
	vecFace[vecLoop[loopLabel].face].loop.push_back(vecLoop.size() - 1);
	vecLoop.back().head = halfEdge1.next;
	vecLoop.back().face = vecLoop[loopLabel].face;

	vecHalfEdge[halfEdge1.last].next = halfEdge2.next;
	vecHalfEdge[halfEdge2.next].last = halfEdge1.last;
	vecHalfEdge[halfEdge1.next].last = halfEdge2.last;
	vecHalfEdge[halfEdge2.last].next = halfEdge1.next;
	printf("Removed edge labeled %d.\n", edgeLabel);
}

void kfmrh(const int &face1Label, const int &face2Label) {
	for (auto i = solid.face.begin(); i != solid.face.end(); i++) {
		if (*i == face1Label) {
			i = solid.face.erase(i);
		}
	}
	printf("Removed face labeled %d.\n", face1Label);
	for (auto i : vecFace[face1Label].loop) {
		vecLoop[i].face = face2Label;
		vecFace[face2Label].loop.push_back(i);
		printf("Loop %d now belongs to face %d.\n", i, face2Label);
	}
}

void sweep(const Point &vector) {
	static std::vector<Command> commands;
	static std::map<int, int> map;
	commands.clear();
	map.clear();
	int pnum = vecPoint.size();
	int fnum = vecFace.size();
	int t = 0;
	for (auto i : vecFace[1].loop) {
		int now = vecLoop[i].head;
		do {
			commands.push_back(Command(1, i, vecHalfEdge[now].b, -1, vecPoint[vecHalfEdge[now].b] + vector));
			map[vecHalfEdge[now].b] = pnum + t++;
			now = vecHalfEdge[now].next;
		} while (now != vecLoop[i].head);
	}
	for (auto i : vecFace[1].loop) {
		int now = vecLoop[i].head;
		do {
			commands.push_back(Command(2, i, map[vecHalfEdge[now].b], map[vecHalfEdge[now].a]));
			now = vecHalfEdge[now].next;
		} while (now != vecLoop[i].head);
	}
	for (int i = 2; i < fnum; i++) {
		commands.push_back(Command(3, i, 0));
	}
	for (auto i : commands) {
		switch (i.type) {
			case 1:
				mev(i.a, i.b, i.d);
				break;
			case 2:
				mef(i.a, i.b, i.c);
				break;
			case 3:
				kfmrh(i.a, i.b);
				break;
			default:
				assert(0);
		}
	}
}

void poly2tri(FILE *out, const int &faceLabel) {
	std::map<int, Point> map;

	for (auto i : vecFace[faceLabel].loop) {
		int now = vecLoop[i].head;
		do {
			map[vecHalfEdge[now].b] = vecPoint[vecHalfEdge[now].b];
			now = vecHalfEdge[now].next;
		} while (now != vecLoop[i].head);
	}

	Point normal;
	{
		Point a, b, c;
		auto i = map.begin();
		a = i->second;
		i++;
		b = i->second;
		i++;
		do {
			c = i->second;
			i++;
		} while (fabs((normal = cross(b - a, c - a)).len2()) < 1e-4);
	}
	int axis = 2;
	if (fabs(normal.x) > fabs(normal.y)) {
		if (fabs(normal.x) > fabs(normal.z)) {
			axis = 0;
		}
	} else if (fabs(normal.y) > fabs(normal.z)) {
		axis = 1;
	}


	std::map<int, p2t::Point> vec;
	for (auto i : map) {
		if (axis == 0) {
			vec[i.first] = p2t::Point(i.second.y, i.second.z);
		} else if (axis == 1) {
			vec[i.first] = p2t::Point(i.second.x, i.second.z);
		} else {
			vec[i.first] = p2t::Point(i.second.x, i.second.y);
		}
	}

	std::vector<p2t::Point*> polyline;
	{
		int now = vecLoop[vecFace[faceLabel].loop[0]].head;
		do {
			polyline.push_back(&vec[vecHalfEdge[now].b]);
			now = vecHalfEdge[now].next;
		} while (now != vecLoop[vecFace[faceLabel].loop[0]].head);
	}
	p2t::CDT cdt(polyline);
	for (auto i : vecFace[faceLabel].loop) {
		if (i == vecFace[faceLabel].loop[0]) {
			continue;
		}
		polyline.clear();
		int now = vecLoop[i].head;
		do {
			polyline.push_back(&vec[vecHalfEdge[now].b]);
			now = vecHalfEdge[now].next;
		} while (now != vecLoop[i].head);
		cdt.AddHole(polyline);
	}
	cdt.Triangulate();
	for (auto i : cdt.GetTriangles()) {
		fprintf(out, "f");
		for (int j = 0; j < 3; j++) {
			p2t::Point p = *i->GetPoint(j);
			for (auto k : vec) {
				if (fabs(p.x - k.second.x) < 1e-4 && fabs(p.y - k.second.y) < 1e-4) {
					fprintf(out, " %d", k.first + 1);
					break;
				}
			}
		}
		fprintf(out, "\n");
	}
}

void generateObj() {
	FILE *out = fopen("a.obj", "w");
	for (auto i : vecPoint) {
		fprintf(out, "v %f %f %f\n", i.x, i.y, i.z);
	}
	for (auto i : solid.face) {
		poly2tri(out, i);
	}
	fclose(out);
}

int _width = 800, _height = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	_width = width;
	_height = height;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void compileShader(unsigned int &shader_program_id) {
	int  success;
	char infoLog[512];

	shader_program_id = glCreateProgram();

	const char *v_str = "#version 330 core\n"
						"layout(location = 0) in vec3 aPos;\n"
						"layout(location = 1) in vec3 aNormal;\n"
						"out vec3 vNormal;\n"
						"uniform mat4 MVP;\n"
						"uniform mat4 View;\n"
						"void main()\n"
						"{\n"
						"    gl_Position = MVP * vec4(aPos, 1.0);\n"
						"    vNormal = normalize((View * vec4(aNormal, 0.0)).xyz);\n"
						"}\n";

	unsigned int vertex_shader_id;
	vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &v_str, NULL);
	glCompileShader(vertex_shader_id);


	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog) << std::endl;
		exit(-1);
	}

	const char *f_str = "#version 330 core\n"
						"in vec3 vNormal;\n"
						"uniform vec4 fillColor;\n"
						"out vec4 FragColor;\n"
						"void main()\n"
						"{\n"
						"    vec3 color = fillColor.xyz * abs(vNormal.z);\n"
						"    FragColor = vec4(color, 1.0);\n"
						"}\n";

	unsigned int fragment_shader_id;
	fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_id, 1, &f_str, NULL);
	glCompileShader(fragment_shader_id);

	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog) << std::endl;
		exit(-1);
	}

	glAttachShader(shader_program_id, vertex_shader_id);
	glAttachShader(shader_program_id, fragment_shader_id);
	glLinkProgram(shader_program_id);

	glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog) << std::endl;
		exit(-1);
	}

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

float *readobj(int &vertex_num, Point &pmin, Point &pmax) {
	static char s[100];
	std::vector<Point> points;
	points.push_back(Point());
	std::vector<Point> faces;
	
	FILE *in = fopen("a.obj", "r");
	while (fgets(s, 100, in)) {
		if (s[0] == 'v') {
			Point p;
			p.read(s + 2);
			points.push_back(p);
		} else if (s[0] == 'f') {
			int a, b, c;
			sscanf(s + 2, "%d%d%d", &a, &b, &c);
			faces.push_back(points[a]);
			faces.push_back(points[b]);
			faces.push_back(points[c]);
		} else {
			assert(0);
		}
	}

	pmin = points[0];
	pmax = points[0];
	for (auto i : points) {
		if (i.x < pmin.x) pmin.x = i.x;
		if (i.y < pmin.y) pmin.y = i.y;
		if (i.z < pmin.z) pmin.z = i.z;

		if (i.x > pmax.x) pmax.x = i.x;
		if (i.y > pmax.y) pmax.y = i.y;
		if (i.z > pmax.z) pmax.z = i.z;
	}

	vertex_num = faces.size();
	float *ret = new float[vertex_num * 6];
	for (int i = 0; i < vertex_num; i += 3) {
		Point normal = cross(faces[i + 1] - faces[i], faces[i + 2] - faces[i]);
		for (int j = 0; j < 3; j++) {
			ret[(i + j) * 6 + 0] = faces[i + j].x;
			ret[(i + j) * 6 + 1] = faces[i + j].y;
			ret[(i + j) * 6 + 2] = faces[i + j].z;
			ret[(i + j) * 6 + 3] = normal.x;
			ret[(i + j) * 6 + 4] = normal.y;
			ret[(i + j) * 6 + 5] = normal.z;
		}
	}
	return ret;
}

void display() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow *window = glfwCreateWindow(800, 600, "view object", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	glViewport(0, 0, _width, _height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int display_polygon_shader = 0;
	compileShader(display_polygon_shader);
	
	Point pmin, pmax;
	int vertex_num;
	float *vertex_data = readobj(vertex_num, pmin, pmax);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * 6 * sizeof(float), vertex_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	Point center = (pmin + pmax) / 2;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 1.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(display_polygon_shader);
		unsigned int uniform_loc = glGetUniformLocation(display_polygon_shader, "MVP");
		
		glm::vec3 up;
		glm::vec3 dir = glm::vec3(0, 0, 1);
		float l = sqrt((pmax - pmin).len2()) / 2;
		float radius = l * 5.f;
		double time = glfwGetTime();
		float camX = sin(time) * radius;
		float camZ = cos(time) * radius;
		glm::vec3 pos;
		static const double PI = acos(-1.0);
		int status =  int(floor(2.0 * time / PI));
		if (status % 8 < 4) {
			pos = glm::vec3(center.x, center.y, center.z) + glm::vec3(camX, 0.0, camZ);
		} else {
			pos = glm::vec3(center.x, center.y, center.z) + glm::vec3(0.0, camX, camZ);
		}
		if (status % 8 < 5 || status % 8 > 6) {
			up = glm::vec3(0, 1, 0);
		} else {
			up = glm::vec3(0, -1, 0);
		}

		auto view = glm::lookAt(pos, glm::vec3(center.x, center.y, center.z), up);

		glm::mat4 mvp = glm::perspective(glm::radians(45.0f), 1.f * _width / _height, 0.1f, 100.0f) * view;

		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, &mvp[0][0]);//
		uniform_loc = glGetUniformLocation(display_polygon_shader, "View");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, &view[0][0]);//
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		uniform_loc = glGetUniformLocation(display_polygon_shader, "fillColor");
		glUniform4f(uniform_loc, 0.8f, 0.8f, 0.8f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertex_num);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete[] vertex_data;
	glDeleteProgram(display_polygon_shader);
	glfwTerminate();
}

int main() {
	static char s[MAXLINELENGTH];
	static char ss[MAXLINELENGTH];
	static int pos;
	FILE *in = stdin;
	while (fgets(s, MAXLINELENGTH, in) != NULL) {
		sscanf(s, "%s", ss);
		std::string str = std::string(ss);
		if (str == "mev") {
			pos = str.length() + 1;
			
			int loopLabel;
			sscanf(s + pos, "%d", &loopLabel);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			int point1Label;
			sscanf(s + pos, "%d", &point1Label);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			Point point;
			point.read(s + pos);

			mev(loopLabel, point1Label, point);
		} else if (str == "mef") {
			pos = str.length() + 1;

			int loopLabel;
			sscanf(s + pos, "%d", &loopLabel);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			int point1Label;
			sscanf(s + pos, "%d", &point1Label);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			int point2Label;
			sscanf(s + pos, "%d", &point2Label);
			
			mef(loopLabel, point1Label, point2Label);
		} else if (str == "mvfs") {
			pos = str.length() + 1;
			Point point;
			point.read(s + pos);

			mvfs(point);
		} else if (str == "kemr") {
			pos = str.length() + 1;

			int loopLabel;
			sscanf(s + pos, "%d", &loopLabel);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			int edgeLabel;
			sscanf(s + pos, "%d", &edgeLabel);
			
			kemr(loopLabel, edgeLabel);
		} else if (str == "kfmrh") {
			pos = str.length() + 1;

			int face1Label;
			sscanf(s + pos, "%d", &face1Label);
			sscanf(s + pos, "%s", ss);
			pos += strlen(ss) + 1;

			int face2Label;
			sscanf(s + pos, "%d", &face2Label);

			kfmrh(face1Label, face2Label);
		} else if (str == "open") {
			sscanf(s, "%*s%s", ss);
			in = fopen(ss, "r");
		} else if (str == "sweep") {
			pos = str.length() + 1;
			Point vector;
			vector.read(s + pos);

			sweep(vector);
		} else if (str == "over") {
			generateObj();
			display();
			break;
		} else if (str == "debug") {
			/*
			printf("solod:\n");
			solid.print();
			for (int i = 0; i < vecFace.size(); i++) {
				printf("face%d:\n", i);
				vecFace[i].print();
			}
			for (int i = 0; i < vecLoop.size(); i++) {
				printf("loop%d:\n", i);
				vecLoop[i].print();
			}
			for (int i = 0; i < vecHalfEdge.size(); i++) {
				printf("halfedge%d:\n", i);
				vecHalfEdge[i].print();
			}
			for (int i = 0; i < vecPoint.size(); i++) {
				printf("point%d:\n", i);
				vecPoint[i].print();
			}

			printf("----------\n");
			*/
			printf("solod:\n");
			solid.print();
			for (int i = 0; i < vecFace.size(); i++) {
				printf("face%d:\n", i);
				for (int j = 0; j < vecFace[i].loop.size(); j++) {
					printf("\tloop%d:\n", vecFace[i].loop[j]);
					int now = vecLoop[vecFace[i].loop[j]].head;
					printf("\t\t%d", vecHalfEdge[now].a);
					do {
						printf(" %d", vecHalfEdge[now].b);
						now = vecHalfEdge[now].next;
					} while (now != vecLoop[vecFace[i].loop[j]].head);
					printf("\n");
				}
			}
		} else {
			assert(0);
		}
	}
	fclose(in);
	return 0;
}