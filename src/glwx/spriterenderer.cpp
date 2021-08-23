#include "spriterenderer.hpp"

#include "shader.hpp"

namespace glwx {

SpriteBatch::SpriteBatch(size_t vertexCount, size_t indexCount)
    : vertexBuffer_(glw::Buffer::Target::Array, glw::Buffer::UsageHint::StreamDraw)
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

    const auto tl = batch_.addVertex(p, region.position, color);
    const auto tr
        = batch_.addVertex(p + sx, region.position + glm::vec2(region.size.x, 0.0f), color);
    const auto br = batch_.addVertex(p + sx + sy, region.position + region.size, color);
    const auto bl
        = batch_.addVertex(p + sy, region.position + glm::vec2(0.0f, region.size.y), color);

    batch_.addIndex(tl);
    batch_.addIndex(bl);
    batch_.addIndex(tr);

    batch_.addIndex(bl);
    batch_.addIndex(br);
    batch_.addIndex(tr);
}

void SpriteRenderer::draw(const glw::Texture& texture, const std::vector<glm::vec2>& points,
    const Transform2D& transform, const TextureRegion& region)
{
    assert(points.size() >= 3);

    setCurrentTexture(&texture);

    const auto baseIdx = batch_.getNumVertices();
    for (const auto& point : points) {
        batch_.addVertex(
            transform.transformPoint(point), point * region.size + region.position, color);
    }

    for (size_t i = 1; i < points.size() - 1; ++i) {
        batch_.addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + 0));
        batch_.addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + i));
        batch_.addIndex(static_cast<SpriteBatch::IndexType>(baseIdx + i + 1));
    }
}

void SpriteRenderer::drawLine(const glw::Texture& texture, const std::vector<glm::vec2>& points,
    float width, bool closed, const Transform2D& transform)
{
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

    for (size_t i = 0; i < (closed ? points.size() : points.size() - 1); ++i) {
        const auto n = (i + 1) % points.size();
        const auto v0 = batch_.addVertex(
            transform.transformPoint(points[i] + pointNormals[i] * width), glm::vec2(i, 1), color);
        const auto v1 = batch_.addVertex(
            transform.transformPoint(points[i] - pointNormals[i] * width), glm::vec2(i, 0), color);
        const auto v2 = batch_.addVertex(
            transform.transformPoint(points[n] - pointNormals[n] * width), glm::vec2(n, 0), color);
        const auto v3 = batch_.addVertex(
            transform.transformPoint(points[n] + pointNormals[n] * width), glm::vec2(n, 1), color);

        batch_.addIndex(v0);
        batch_.addIndex(v1);
        batch_.addIndex(v3);

        batch_.addIndex(v1);
        batch_.addIndex(v2);
        batch_.addIndex(v3);
    }
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
