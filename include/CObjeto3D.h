class CObjeto3D{


private:

float *verts;
int *inds;
float *norms;

vector<glm::vec4> vertices;
vector<glm::vec3> normals;
vector<GLushort> elements;

GLuint _myVBO,_myIndices,_myNormals;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

void escreveinfo(std::string err){
    if (!err.empty()) {
      std::cerr << err << std::endl;
      exit(1);
    }

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;

    for (size_t i = 0; i < shapes.size(); i++) {
      printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
      printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
      printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
      assert((shapes[i].mesh.indices.size() % 3) == 0);
      for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
        printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
      }

      printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
      assert((shapes[i].mesh.positions.size() % 3) == 0);
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
        printf("  v[%ld] = (%f, %f, %f)\n", v,
          shapes[i].mesh.positions[3*v+0],
          shapes[i].mesh.positions[3*v+1],
          shapes[i].mesh.positions[3*v+2]);
      }
    }

    for (size_t i = 0; i < materials.size(); i++) {
      printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
      printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
      printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
      printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
      printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
      printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
      printf("  material.Ns = %f\n", materials[i].shininess);
      printf("  material.Ni = %f\n", materials[i].ior);
      printf("  material.dissolve = %f\n", materials[i].dissolve);
      printf("  material.illum = %d\n", materials[i].illum);
      printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
      printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
      printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
      printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
      std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
      std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
      for (; it != itEnd; it++) {
        printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
      }
      printf("\n");
    }
}

int sizeverts,sizeinds,sizenorms;

public:

    CObjeto3D(char* filename){
        std::string err = tinyobj::LoadObj(shapes, materials, filename);
        //escreveinfo(err);

        sizeverts = shapes[0].mesh.positions.size()*sizeof(float);
        printf("verts: %d   size: %d\n",sizeverts,shapes[0].mesh.positions.size());
        verts = (float*) malloc(sizeverts);
        printf("malocado\n");
        memcpy(verts,(void*) &shapes[0].mesh.positions[0],sizeverts);
        printf("copiado\n");

        sizeinds = shapes[0].mesh.indices.size()*sizeof(int);
        printf("inds: %d   size: %d\n",sizeinds,shapes[0].mesh.indices.size());
        inds = (int*) malloc(sizeinds);
        printf("malocado\n");
        memcpy(inds,(void*) &shapes[0].mesh.indices[0],sizeinds);
        printf("copiado\n");

        sizenorms = shapes[0].mesh.positions.size()*sizeof(float)*3;
        printf("norms: %d   size: %d\n",sizenorms,shapes[0].mesh.positions.size());
        norms = (float*) malloc(sizenorms);
        printf("malocado\n");
        //memcpy(norms,(void*) &shapes[0].mesh.indices[0],sizeinds);
        printf("copiado\n");


  for (int i = 0; i < shapes[0].mesh.indices.size(); i+=3) {
    GLuint ia = 3*inds[i];
    GLuint ib = 3*inds[i+1];
    GLuint ic = 3*inds[i+2];
    glm::vec3 v1,v2,v3;
    v1.x = verts[ia];
    v1.y = verts[ia+1];
    v1.z = verts[ia+2];
    v2.x = verts[ib];
    v2.y = verts[ib+1];
    v2.z = verts[ib+2];
    v3.x = verts[ic];
    v3.y = verts[ic+1];
    v3.z = verts[ic+2];
    glm::vec3 normal = glm::normalize(glm::cross(
      v2 - v1,
      v3 - v1));
    norms[ia] = norms[ib] = norms[ic] = normal.x;
    norms[ia+1] = norms[ib+1] = norms[ic+1] = normal.y;
    norms[ia+2] = norms[ib+2] = norms[ic+2] = normal.z;
  }

        glGenBuffers(1, &_myVBO);
        glGenBuffers(1, &_myIndices);
        glGenBuffers(1, &_myNormals);

        glBindBuffer(GL_ARRAY_BUFFER, _myVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeverts, verts, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeinds, inds, GL_STATIC_DRAW);

        glBindBuffer(GL_NORMAL_ARRAY, _myNormals);
        glBufferData(GL_NORMAL_ARRAY, sizenorms, norms, GL_STATIC_DRAW);
    }

    void desenha(){
        glBindBuffer(GL_ARRAY_BUFFER, _myVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndices);

        glEnableVertexAttribArray(0);                                               //ENABLE VERTEX POSITION
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);      // VERTEX POSITION POINTER


glBindBuffer(GL_ARRAY_BUFFER, _myNormals);
  glVertexAttribPointer(
    0, // attribute
    3,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );

        glDrawElements(GL_TRIANGLES, sizeinds/(sizeof(int)), GL_UNSIGNED_INT, 0);
    }

/*void load_obj(const char* filename ) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

  string line;
  while (getline(in, line)) {
    if (line.substr(0,2) == "v ") {
      istringstream s(line.substr(2));
      glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
      vertices.push_back(v);
    }  else if (line.substr(0,2) == "f ") {
      istringstream s(line.substr(2));
      GLushort a,b,c;
      s >> a; s >> b; s >> c;
      a--; b--; c--;
      elements.push_back(a); elements.push_back(b); elements.push_back(c);
    }
    else if (line[0] == '#') { /* ignoring this line  }
    else { /* ignoring this line  }
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

  glGenBuffers(1, &myVBO);
    glGenBuffers(1, &myIndices);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices->, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndices);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), elements, GL_STATIC_DRAW);
}


void desenha(){
    glEnableVertexAttribArray(attribute_v_coord);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
  glVertexAttribPointer(
    attribute_v_coord,  // attribute
    4,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
  glVertexAttribPointer(
    attribute_v_normal, // attribute
    3,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements);
  int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);


}*/

};


typedef CObjeto3D* Objeto3D;
