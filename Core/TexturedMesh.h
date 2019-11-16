#ifndef STRETCH_TEXTURE_H
#define STRETCH_TEXTURE_H

#include "SolidGroundLine.h"

class TexturedMesh : public BeautyBase
{
public:
	TexturedMesh(rapidxml::xml_node<> *xe);
	TexturedMesh(const TexturedMesh &l);
	TexturedMesh(const std::string &textureName, const FPoint2D pos, const Besier &besier);
	virtual std::string Type() const;
	//virtual bool PixelCheck(const FPoint2D &point);
	virtual void Draw();
	virtual void ReloadTextures(bool touch = false);
	void ShiftU(float u);
private:
	GLTexture *_texture;
	std::string _fileName;
	VertexBuffer _vb;
	FPoint2D _min;
	FPoint2D _max;
	void SearchMinMax();
};

#endif

