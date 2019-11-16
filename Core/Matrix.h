#ifndef MYENGINE_MATRIX_H
#define MYENGINE_MATRIX_H

#include "FPoint2D.h"

struct Matrix {
private:
	float _matrix[3][3];
	void Mul(const float m[3][3]) {
		float tmp[2][3];
		memcpy(tmp, _matrix, sizeof(tmp));
        _matrix[0][0] = m[0][0] * tmp[0][0] + m[0][1] * tmp[1][0] + m[0][2] * _matrix[2][0];
        _matrix[0][1] = m[0][0] * tmp[0][1] + m[0][1] * tmp[1][1] + m[0][2] * _matrix[2][1];
        _matrix[0][2] = m[0][0] * tmp[0][2] + m[0][1] * tmp[1][2] + m[0][2] * _matrix[2][2];

        _matrix[1][0] = m[1][0] * tmp[0][0] + m[1][1] * tmp[1][0] + m[1][2] * _matrix[2][0];
        _matrix[1][1] = m[1][0] * tmp[0][1] + m[1][1] * tmp[1][1] + m[1][2] * _matrix[2][1];
        _matrix[1][2] = m[1][0] * tmp[0][2] + m[1][1] * tmp[1][2] + m[1][2] * _matrix[2][2];
        
        _matrix[2][0] = m[2][0] * tmp[0][0] + m[2][1] * tmp[1][0] + m[2][2] * _matrix[2][0];
        _matrix[2][1] = m[2][0] * tmp[0][1] + m[2][1] * tmp[1][1] + m[2][2] * _matrix[2][1];
        _matrix[2][2] = m[2][0] * tmp[0][2] + m[2][1] * tmp[1][2] + m[2][2] * _matrix[2][2];
	}
public:
	float &Index(int i, int j) { return _matrix[i][j]; }
	void Unit() {
		_matrix[0][0] = 1.f; _matrix[0][1] = 0.f; _matrix[0][2] = 0.f;
		_matrix[1][0] = 0.f; _matrix[1][1] = 1.f; _matrix[1][2] = 0.f;
		_matrix[2][0] = 0.f; _matrix[2][1] = 0.f; _matrix[2][2] = 1.f;
	}
	Matrix() {
		Unit();
	}
	void Rotate(float angle) {
        float ca(sin(angle + 1.570796f));
        float sa(sin(angle));

        float m0(ca * _matrix[0][0] + sa * _matrix[1][0]);
        float m1(ca * _matrix[0][1] + sa * _matrix[1][1]);
        float m2(ca * _matrix[0][2] + sa * _matrix[1][2]);
        
        _matrix[1][0] = ca * _matrix[1][0] - sa * _matrix[0][0];
        _matrix[1][1] = ca * _matrix[1][1] - sa * _matrix[0][1];
        _matrix[1][2] = ca * _matrix[1][2] - sa * _matrix[0][2];
        
        _matrix[0][0] = m0;
        _matrix[0][1] = m1;
        _matrix[0][2] = m2;
    }
	void Scale(float scx, float scy) {
        _matrix[0][0] *= scx;
        _matrix[0][1] *= scx;
        _matrix[0][2] *= scx;
        
        _matrix[1][0] *= scy;
        _matrix[1][1] *= scy;
        _matrix[1][2] *= scy;
    }
	void Move(float x, float y) {
        _matrix[2][0] += (x * _matrix[0][0] + y * _matrix[1][0]);
        _matrix[2][1] += (x * _matrix[0][1] + y * _matrix[1][1]);
        _matrix[2][2] += (x * _matrix[0][2] + y * _matrix[1][2]);
	
    }
	inline void Mul(FPoint2D origin, float &x, float &y) const {
		x = _matrix[0][0] * origin.x + _matrix[1][0] * origin.y + _matrix[2][0];
		y = _matrix[0][1] * origin.x + _matrix[1][1] * origin.y + _matrix[2][1];
	}
    inline void Mul(const FPoint2D &origin, FPoint2D &target) const {
        assert(&origin != &target);
        target.x = _matrix[0][0] * origin.x + _matrix[1][0] * origin.y + _matrix[2][0];
        target.y = _matrix[0][1] * origin.x + _matrix[1][1] * origin.y + _matrix[2][1];
    }
    void Mul(const Matrix &transform) {
		Mul(transform._matrix);
	}
	inline void Mul(FPoint2D &p) const {
		float x = p.x;
		p.x = _matrix[0][0] * x + _matrix[1][0] * p.y + _matrix[2][0];
		p.y = _matrix[0][1] * x + _matrix[1][1] * p.y + _matrix[2][1];
	}
	inline void Mul(float &x, float &y) const {
		float tx = x;
		x = _matrix[0][0] * tx + _matrix[1][0] * y + _matrix[2][0];
		y = _matrix[0][1] * tx + _matrix[1][1] * y + _matrix[2][1];
	}
	inline void Mul(float sx, float sy, float &x, float &y) const {
		x = _matrix[0][0] * sx + _matrix[1][0] * sy + _matrix[2][0];
		y = _matrix[0][1] * sx + _matrix[1][1] * sy + _matrix[2][1];
	}
	float Determinant() const {
		return _matrix[0][0]*_matrix[1][1]*_matrix[2][2] 
					- _matrix[0][0]*_matrix[1][2]*_matrix[2][1] 
					- _matrix[0][1]*_matrix[1][0]*_matrix[2][2] 
					+ _matrix[0][1]*_matrix[1][2]*_matrix[2][0] 
					+ _matrix[0][2]*_matrix[1][0]*_matrix[2][1] 
					- _matrix[0][2]*_matrix[1][1]*_matrix[2][0];
	}
	void Flip() {
		float t;
		
		t = _matrix[1][0];
		_matrix[1][0] = _matrix[0][1];
		_matrix[0][1] = t;

		t = _matrix[1][2];
		_matrix[1][2] = _matrix[2][1];
		_matrix[2][1] = t;

		t = _matrix[2][0];
		_matrix[2][0] = _matrix[0][2];
		_matrix[0][2] = t;
	}
	void Mul(float f) {
		_matrix[0][0] *= f;
		_matrix[1][0] *= f;
		_matrix[2][0] *= f;

		_matrix[0][1] *= f;
		_matrix[1][1] *= f;
		_matrix[2][1] *= f;

		_matrix[0][2] *= f;
		_matrix[1][2] *= f;
		_matrix[2][2] *= f;
	}
	void MakeRevers(const Matrix &transform) {
		float det = transform.Determinant();

		_matrix[0][0] = (transform._matrix[1][1] * transform._matrix[2][2] - transform._matrix[2][1] * transform._matrix[1][2]);
		_matrix[1][0] = -(transform._matrix[0][1] * transform._matrix[2][2] - transform._matrix[2][1] * transform._matrix[0][2]);
		_matrix[2][0] = (transform._matrix[0][1] * transform._matrix[1][2] - transform._matrix[1][1] * transform._matrix[0][2]);

		_matrix[0][1] = -(transform._matrix[1][0] * transform._matrix[2][2] - transform._matrix[2][0] * transform._matrix[1][2]);
		_matrix[1][1] = (transform._matrix[0][0] * transform._matrix[2][2] - transform._matrix[2][0] * transform._matrix[0][2]);
		_matrix[2][1] = -(transform._matrix[0][0] * transform._matrix[1][2] - transform._matrix[1][0] * transform._matrix[0][2]);

		_matrix[0][2] = (transform._matrix[1][0] * transform._matrix[2][1] - transform._matrix[2][0] * transform._matrix[1][1]);
		_matrix[1][2] = -(transform._matrix[0][0] * transform._matrix[2][1] - transform._matrix[2][0] * transform._matrix[0][1]);
		_matrix[2][2] = (transform._matrix[0][0] * transform._matrix[1][1] - transform._matrix[1][0] * transform._matrix[0][1]);

		Flip();

		Mul(1.f / det);
	}
	void GetScale(float &sx, float &sy) {
		sx = _matrix[0][0];
		sy = _matrix[1][1];
	}
};

#endif//MYENGINE_MATRIX_H
