struct Mesh
{
    Vertex* vertices;
    uint32 num_vertices;
    uint32* indices;
    uint32 num_indices;
};

struct LoadedMesh
{
    bool valid;
    Mesh mesh;
};

struct ParserState
{
    uint8* data;
    uint8* head;
    uint8* end;
};

namespace obj
{

namespace internal
{

struct ParsedFace
{
    uint32 v1;
    uint32 v2;
    uint32 v3;
    uint32 n1;
    uint32 n2;
    uint32 n3;
    uint32 u1;
    uint32 u2;
    uint32 u3;
};

struct ParsedData
{
    Vector3* vertices;
    Vector2* uvs;
    Vector3* normals;
    ParsedFace* faces;
    uint32 num_vertices;
    uint32 num_uvs;
    uint32 num_normals;
    uint32 num_faces;
};

void skip_to_numeric(ParserState* ps)
{
    while (ps->head < ps->end && (*ps->head < '0' || *ps->head > '9') && *ps->head != '-')
    {
        ++ps->head;
    }
}

void parse_uv(ParserState* ps, ParsedData* pd)
{
    Vector2 uv;
    skip_to_numeric(ps);
    uv.x = strtof((const char*)ps->head, (char**)&ps->head);
    skip_to_numeric(ps);
    uv.y = strtof((const char*)ps->head, (char**)&ps->head);
    pd->uvs[pd->num_uvs++] = uv;
}

void parse_normal(ParserState* ps, ParsedData* pd)
{
    Vector3 normal;
    skip_to_numeric(ps);
    normal.x = strtof((const char*)ps->head, (char**)&ps->head);
    skip_to_numeric(ps);
    normal.y = strtof((const char*)ps->head, (char**)&ps->head);
    skip_to_numeric(ps);
    normal.z = strtof((const char*)ps->head, (char**)&ps->head);
    pd->normals[pd->num_normals++] = normal;
}

void parse_vertex(ParserState* ps, ParsedData* pd)
{
    Vector3 vertex;
    skip_to_numeric(ps);
    vertex.x = strtof((const char*)ps->head, (char**)&ps->head);
    skip_to_numeric(ps);
    vertex.y = strtof((const char*)ps->head, (char**)&ps->head);
    skip_to_numeric(ps);
    vertex.z = strtof((const char*)ps->head, (char**)&ps->head);
    pd->vertices[pd->num_vertices++] = vertex;
}

void parse_face(ParserState* ps, ParsedData* pd)
{
    ParsedFace face;
    skip_to_numeric(ps);
    face.v1 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.u1 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.n1 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.v2 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.u2 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.n2 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.v3 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.u3 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    skip_to_numeric(ps);
    face.n3 = strtol((const char*)ps->head, (char**)&ps->head, 10) - 1;
    pd->faces[pd->num_faces++] = face;
}

void skip_line(ParserState* ps)
{
    while (ps->head < ps->end && *ps->head != '\n')
    {
        ++ps->head;
    }
}

ParsedData parse(Allocator* alloc, uint8* data, uint32 data_size)
{
    ParserState ps = {0};
    ps.data = data;
    ps.head = data;
    ps.end = (uint8*)memory::ptr_add(data, data_size);
    ParsedData pd = {0};
    pd.vertices = (Vector3*)alloc->alloc(sizeof(Vector3) * 128);
    pd.uvs = (Vector2*)alloc->alloc(sizeof(Vector2) * 128);
    pd.normals = (Vector3*)alloc->alloc(sizeof(Vector3) * 128);
    pd.faces = (internal::ParsedFace*)alloc->alloc(sizeof(Vector3) * 128);

    while (ps.head <  ps.end)
    {
        uint8 c = *ps.head;

        if (c == 'v' && ps.head + 1 < ps.end && (*(ps.head+1)) == 't')
            parse_uv(&ps, &pd);
        else if (c == 'v' && ps.head + 1 < ps.end && (*(ps.head+1)) == 'n')
            parse_normal(&ps, &pd);
        else if (c == 'v')
            parse_vertex(&ps, &pd);
        else if (c == 'f')
            parse_face(&ps, &pd);
        else if (c == '#' || c == 'o' || c == 's')
            skip_line(&ps);
        else
            ++ps.head;
    }

    return pd;
}

int32 get_existing_vertex(Vertex* vertices, uint32 num_vertices, const Vertex& v1)
{
    for (uint32 i = 0; i < num_vertices; ++i)
    {
        const Vertex& v2 = vertices[i];

        if (vector3::almost_equal(v1.position, v2.position)
            && vector3::almost_equal(v1.normal, v2.normal)
            && vector2::almost_equal(v1.uv, v2.uv)
            && color::almost_equal(v1.color, v2.color))
        {
            return i;
        }
    }

    return -1;
}

void add_vertex_to_mesh(Mesh* m, const Vertex& v)
{
    int32 i = get_existing_vertex(m->vertices, m->num_vertices, v);

    if (i != -1)
    {
        m->indices[m->num_indices++] = i;
        return;
    }

    m->indices[m->num_indices++] = m->num_vertices;
    m->vertices[m->num_vertices++] = v;
}

}

LoadedMesh load(Allocator* alloc, const char* filename)
{
    LoadedFile lf = file::load(alloc, filename);

    if (!lf.valid)
        return {false};

    internal::ParsedData pd = internal::parse(alloc, lf.file.data, lf.file.size);
    Mesh m = {0};
    m.vertices = (Vertex*)alloc->alloc(sizeof(Vertex) * 100);
    m.indices = (uint32*)alloc->alloc(sizeof(uint32) * 100);

    for (uint32 i = 0; i < pd.num_faces; ++i)
    {
        const internal::ParsedFace& f = pd.faces[i];
        internal::add_vertex_to_mesh(&m, {pd.vertices[f.v1], pd.normals[f.n1], pd.uvs[f.u1], {1.0f, 1.0f, 1.0f, 1.0f}});
        internal::add_vertex_to_mesh(&m, {pd.vertices[f.v2], pd.normals[f.n2], pd.uvs[f.u2], {1.0f, 1.0f, 1.0f, 1.0f}});
        internal::add_vertex_to_mesh(&m, {pd.vertices[f.v3], pd.normals[f.n3], pd.uvs[f.u3], {1.0f, 1.0f, 1.0f, 1.0f}});
    }

    return {true, m};
}

}