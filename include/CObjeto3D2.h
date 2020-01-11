class CObjeto3D{


private:

vector<glm::vec3> vertices;
vector<glm::vec3> normals;
vector<GLushort> elements;

GLuint _myVBO,_myIndices,_myNormals;


public:

    CObjeto3D(char* filename){
        /*glGenBuffers(1, &_myVBO);
        glGenBuffers(1, &_myIndices);
        glGenBuffers(1, &_myNormals);

        glBindBuffer(GL_ARRAY_BUFFER, _myVBO);
        glBindBuffer(GL_NORMAL_ARRAY, _myNormals);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndices);
        */
        load_obj(filename);
    }


void load_obj(const char* filename ) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

  string line;
  while (getline(in, line)) {
    if (line.substr(0,2) == "v ") {
      istringstream s(line.substr(2));
      glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
      printf("%f %f %f \n",v.x,v.y,v.z);
      vertices.push_back(v);
    }  else if (line.substr(0,2) == "f ") {
      istringstream s(line.substr(2));
      GLushort a,b,c,d;
      s >> a; s >> b; s >> c; s>> d;
      a--; b--; c--; d--;
      printf("%d %d %d %d\n",a,b,c,d);
      elements.push_back(a); elements.push_back(b); elements.push_back(c);elements.push_back(d);
    }
    else if (line[0] == '#') { /* ignoring this line*/  }
    else { /* ignoring this line */ }
  }

  normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
  for (int i = 0; i < elements.size(); i+=3) {
    GLushort ia = elements[i];
    GLushort ib = elements[i+1];
    GLushort ic = elements[i+2];
    glm::vec3 normal = glm::normalize(glm::cross(
      glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
      glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
    normals[ia] = normals[ib] = normals[ic] = normal;
  }

    glGenBuffers(1, &_myVBO);
    glGenBuffers(1, &_myIndices);
    //glGenBuffers(1, &_myNormals);
    glBindBuffer(GL_ARRAY_BUFFER, _myVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(int), &elements[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_NORMAL_ARRAY, _myNormals);
    //glBufferData(GL_NORMAL_ARRAY, normals.size()*3, &normals[0], GL_STATIC_DRAW);
}


void desenha(){
    glEnableVertexAttribArray(0);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, _myVBO);
  glVertexAttribPointer(
    0,  // attribute
    3,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );


  //glBindBuffer(GL_NORMAL_ARRAY, _myNormals);
  /*glVertexAttribPointer(
    0, // attribute
    3,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    sizeof(float)*3,                  // no extra data between each position
    0                   // offset of first element
  );*/

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndices);
  int size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_QUADS, elements.size(), GL_UNSIGNED_SHORT, 0);


}

};


typedef CObjeto3D* Objeto3D;

