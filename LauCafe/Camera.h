#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "Vector3.h"
#include "Main.h"

class Camera
{
public:
	Camera();
	virtual ~Camera();

	inline void setEye(float x, float y, float z);
	inline void setEye(const Vector3& eye);
	inline void setUp(float x, float y, float z);
	inline void setUp(const Vector3& up);
	inline void setViewDir(float x, float y, float z);
	inline void setViewDir(const Vector3& vd);
	inline void setLookAt(float x, float y, float z);
	inline void setLookAt(const Vector3& look);
	inline void setFOV(float fov) { m_Fov = fov; }

	inline float fov() const                { return m_Fov; }
	inline const Vector3 & viewDir() const  { return m_ViewDir; }
	inline const Vector3 & lookAt() const   { return m_LookAt; }
	inline const Vector3 & up() const       { return m_Up; }
	inline const Vector3 & eye() const      { return m_Eye; }

	void drawGL();
private:
	void calcLookAt();

	Vector3 m_Eye;
	Vector3 m_Up;
	Vector3 m_ViewDir;
	Vector3 m_LookAt;
	float m_Fov;
};

extern Camera * g_Camera;

inline void Camera::setEye(float x, float y, float z)
{
	m_Eye.set(x, y, z);
}

inline void Camera::setEye(const Vector3& eye)
{
	setEye(eye.x, eye.y, eye.z);
}

inline void Camera::setUp(float x, float y, float z)
{
	m_Up.set(x, y, z);
	m_Up.normalize();
}

inline void Camera::setUp(const Vector3& up)
{
	setUp(up.x, up.y, up.z);
}

inline void Camera::setViewDir(float x, float y, float z)
{
	m_ViewDir.set(x, y, z);
	m_ViewDir.normalize();
}

inline void Camera::setViewDir(const Vector3& vd)
{
	setViewDir(vd.x, vd.y, vd.z);
}

inline void Camera::setLookAt(float x, float y, float z)
{
	Vector3 dir = Vector3(x, y, z) - m_Eye;
	setViewDir(dir);
}

inline void Camera::setLookAt(const Vector3& vd)
{
	setLookAt(vd.x, vd.y, vd.z);
}


#endif