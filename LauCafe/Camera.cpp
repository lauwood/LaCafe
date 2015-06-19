#include "Camera.h"

Camera * g_Camera = 0;

Camera::Camera() :
	m_Eye(0, 0, 0),
	m_ViewDir(0, 0, -1),
	m_Up(0, 1, 0),
	m_LookAt(FLT_MAX, FLT_MAX, FLT_MAX),
	m_Fov((45.)*(M_PI / 180.))
{
	calcLookAt();
}

Camera::~Camera()
{

}

void
Camera::calcLookAt()
{
	if (m_LookAt.x != FLT_MAX)
	{
		setLookAt(m_LookAt);
		m_LookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
	}
}

void
Camera::drawGL()
{
	// set up the screen with our camera parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov(), WIN_X / (float)WIN_Y,
		0.01, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3 vCenter = eye() + viewDir();
	gluLookAt(eye().x, eye().y, eye().z,
		vCenter.x, vCenter.y, vCenter.z,
		up().x, up().y, up().z);
}