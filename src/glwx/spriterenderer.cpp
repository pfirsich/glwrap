#include "spriterenderer.hpp"

#include "shader.hpp"

namespace glwx {

SpriteBatch::SpriteBatch(size_t vertexCount, size_t indexCount)
    : primitive_(glw::DrawMode::Triangles)
    , vertexBuffer_(glw::Buffer::Target::Array, glw::Buffer::UsageHint::StreamDraw)
    , indexBuffer_(glw::Buffer::Target::ElementArray, glw::Buffer::UsageHint::StreamDraw)
{
    vertices_.reserve(vertexCount);
    indices_.reserve(indexCount > 0 ? indexCount : vertexCount);

    primitive_.addVertexBuffer(vertexBuffer_, getVertexFormat());
    primitive_.setIndexBuffer(indexBuffer_, glw::IndexEnum<IndexType>);
}

SpriteBatch::IndexType SpriteBatch::addVertex(
    const glm::vec2& position, const glm::vec2& texCoord, const glm::vec4& color)
{
    vertices_.push_back(Vertex { position, texCoord, color });
    const auto index = vertices_.size() - 1;
    assert(index <= std::numeric_limits<IndexType>::max());
    return static_cast<IndexType>(index);
}

void SpriteBatch::addIndex(IndexType index)
{
    indices_.push_back(index);
}

size_t SpriteBatch::getNumVertices() const
{
    return vertices_.size();
}

size_t SpriteBatch::getNumIndices() const
{
    return indices_.size();
}

void SpriteBatch::clear()
{
    vertices_.clear();
    indices_.clear();
}

void SpriteBatch::flush()
{
    if (!indices_.empty()) {
        vertexBuffer_.update(vertices_);
        indexBuffer_.update(indices_);
        primitive_.draw(0, indices_.size());
    }
    clear();
}

const glw::VertexFormat& SpriteBatch::getVertexFormat()
{
    static glw::VertexFormat vfmt;
    if (vfmt.getStride() == 0) {
        vfmt.add(AttributeLocations::Position, 2, glw::AttributeType::F32);
        vfmt.add(AttributeLocations::TexCoord, 2, glw::AttributeType::F32);
        vfmt.add(AttributeLocations::Color, 4, glw::AttributeType::F32);
    }
    return vfmt;
}

glw::ShaderProgram& SpriteRenderer::getDefaultShaderProgram()
{
    static auto shader = makeShaderProgram(defaultVertexShader, defaultFragmentShader).value();
    return shader;
}

void SpriteRenderer::setCurrentTexture(const glw::Texture* texture)
{
    if (currentTexture_ != texture) {
        if (currentTexture_) {
            flush();
        }
        currentTexture_ = texture;
    }
}

void SpriteRenderer::draw(
    const glw::Texture& texture, const Transform2D& transform, const TextureRegion& region)
{
    setCurrentTexture(&texture);

    const auto p = transform.transformPoint(glm::vec2(0.0f, 0.0f));
    const auto sx
        = transform.transformDirection(glm::vec2(static_cast<float>(texture.getWidth()), 0.0f));
    const auto sy
        = transform.transformDirection(glm::vec2(0.0f, static_cast<float>(texture.getHeight())));

    const auto tl = addVertex(p, region.position);
    const auto tr = addVertex(p + sx, region.position + glm::vec2(region.size.x, 0.0f));
    const auto br = addVertex(p + sx + sy, region.position + region.size);
    const auto bl = addVertex(p + sy, region.position + glm::vec2(0.0f, region.size.y));

    addIndex(tl);
    addIndex(bl);
    addIndex(tr);

    addIndex(bl);
    addIndex(br);
    addIndex(tr);
}

void SpriteRenderer::draw(const glw::Texture& texture, const std::vector<glm::vec2>& points,
    const Transform2D& transform, const TextureRegion& region)
{
    assert(points.size() >= 3);

    setCurrentTexture(&texture);

    const auto baseIdx = getVertexOffset();
    for (const auto& point : points) {
        addVertex(transform.transformPoint(point), point * region.size + region.position);
    }

    for (size_t i = 1; i < points.size() - 1; ++i) {
        addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + 0));
        addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + i));
        addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + i + 1));
    }
}

void SpriteRenderer::drawLine(const glw::Texture& texture, const std::vector<glm::vec2>& points,
    float width, bool closed, const Transform2D& transform)
{
    assert(points.size() >= 2);

    setCurrentTexture(&texture);

    std::vector<glm::vec2> edgeNormals(closed ? points.size() : points.size() - 1);
    for (size_t i = 0; i < edgeNormals.size(); ++i) {
        const auto n = (i + 1) % points.size();
        const auto rel = points[n] - points[i];
        // For a counter-clockwise polygon, these should point outside
        edgeNormals[i] = glm::normalize(glm::vec2(-rel.y, rel.x));
    }

    std::vector<glm::vec2> pointNormals(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        if (!closed && (i == 0 || i == points.size() - 1)) {
            pointNormals[i] = edgeNormals[i];
        } else {
            const auto prev = i == 0 ? points.size() - 1 : i - 1;
            pointNormals[i] = glm::normalize(edgeNormals[i] + edgeNormals[prev]);
        }
    }

    const auto hWidth = width / 2.0f;
    for (size_t i = 0; i < (closed ? points.size() : points.size() - 1); ++i) {
        const auto n = (i + 1) % points.size();
        const auto v0 = addVertex(
            transform.transformPoint(points[i] + pointNormals[i] * hWidth), glm::vec2(i, 1));
        const auto v1 = addVertex(
            transform.transformPoint(points[i] - pointNormals[i] * hWidth), glm::vec2(i, 0));
        const auto v2 = addVertex(
            transform.transformPoint(points[n] - pointNormals[n] * hWidth), glm::vec2(n, 0));
        const auto v3 = addVertex(
            transform.transformPoint(points[n] + pointNormals[n] * hWidth), glm::vec2(n, 1));

        addIndex(v0);
        addIndex(v1);
        addIndex(v3);

        addIndex(v1);
        addIndex(v2);
        addIndex(v3);
    }
}

void SpriteRenderer::drawCircle(const glw::Texture& texture, const glm::vec2& position,
    float radius, size_t segments, const Transform2D& transform)
{
    assert(segments >= 3);

    setCurrentTexture(&texture);

    const auto base = addVertex(transform.transformPoint(position), glm::vec2(0.5f));
    for (size_t i = 0; i < segments; ++i) {
        const auto angle = 2.0f * glm::pi<float>() / (segments - 1) * i;
        const auto v = glm::vec2(std::cos(angle), std::sin(angle));
        addVertex(transform.transformPoint(position + v * radius), glm::vec2(0.5f) + v * 0.5f);
        addIndex(base);
        addIndex(base + 1 + i);
        addIndex(base + 1 + ((i + 1) % segments));
    }
}

SpriteBatch::IndexType SpriteRenderer::getVertexOffset() const
{
    return batch_.getNumVertices();
}

SpriteBatch::IndexType SpriteRenderer::addVertex(
    const glm::vec2& position, const glm::vec2& texCoord)
{
    return batch_.addVertex(position, texCoord, color);
}

void SpriteRenderer::addIndex(SpriteBatch::IndexType index)
{
    batch_.addIndex(index);
}

void SpriteRenderer::flush()
{
    // This was an assert(currentTexture_) before and it should not be nullptr, except if the
    // SpriteRenderer was not used yet.
    if (currentTexture_) {
        // We could (should!) be cleverer with allocating texture units here
        currentTexture_->bind(0);
        batch_.flush();
    }
}

}
