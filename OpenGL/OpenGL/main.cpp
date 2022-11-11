#include <GL/glut.h>
#include "he.h"

#define PI 3.1415926536
#define PRINT_COOD(cood) printf("%.0f %.0f %.0f\n", cood[0], cood[1], cood[2]);

GLUtesselator* obj = gluNewTess();
Solid* solid = new Solid();
double xrot = 0.0;
double yrot = 0.0;
double xrotr = 0.0;
double yrotr = 0.0;
int xp, yp;
bool bdown = false;

void DrawEdges(Edge* edge)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3d(edge->half_l->sv->coordinate[0], edge->half_l->sv->coordinate[1], edge->half_l->sv->coordinate[2]);
	glVertex3d(edge->half_l->ev->coordinate[0], edge->half_l->ev->coordinate[1], edge->half_l->ev->coordinate[2]);
	glEnd();
}

void DrawFace(Face* face)
{
	Loop* out_lp = face->out_lp;
	HalfEdge* he = out_lp->halfedges;
	glColor3f(0.9, 0.3, 0.2);
	gluTessBeginPolygon(obj, NULL);
	gluTessBeginContour(obj);
	while (he)
	{
		gluTessVertex(obj, he->sv->coordinate, he->sv->coordinate);
		he = he->next;
		if (he == out_lp->halfedges)
		{
			break;
		}
	}
	gluTessEndContour(obj);
	Loop* in_lp = face->inner_lp;
	while (in_lp)
	{
		gluTessBeginContour(obj);
		he = in_lp->halfedges;
		while (he)
		{
			gluTessVertex(obj, he->sv->coordinate, he->sv->coordinate);
			he = he->next;
			if (he == in_lp->halfedges)
			{
				break;
			}
		}
		gluTessEndContour(obj);
		in_lp = in_lp->next;
	}
	gluTessEndPolygon(obj);
}

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-0.0, 0.0, -10.0);
	glRotatef(0.0, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	Face* face = solid->faces;
	while (face)
	{
		DrawFace(face);
		face = face->next;
	}
	Edge* edge = solid->edges;
	while (edge)
	{
		DrawEdges(edge);
		edge = edge->next;
	}
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void creatSolid()
{
	double pA[3] = { 2.0, -2.0, 0.0 };
	double pB[3] = { 2.0, 2.0, 0.0 };
	double pC[3] = { -2.0, 2.0, 0.0 };
	double pD[3] = { -2.0, -2.0, 0.0 };
	double pA_[3] = { 2.0, -2.0, 1.0 };
	double pB_[3] = { 2.0, 2.0, 1.0 };
	double pC_[3] = { -2.0, 2.0, 1.0 };
	double pD_[3] = { -2.0, -2.0, 1.0 };
	double pE[3] = { 1.0, -1.0, 0.0 };
	double pF[3] = { 1.0, 1.0, 0.0 };
	double pG[3] = { -1.0, 1.0, 0.0 };
	double pH[3] = { -1.0, -1.0, 0.0 };
	double pE_[3] = { 1.0, -1.0, 1.0 };
	double pF_[3] = { 1.0, 1.0, 1.0 };
	double pG_[3] = { -1.0, 1.0, 1.0 };
	double pH_[3] = { -1.0, -1.0, 1.0 };
	solid = mvfs(pA);
	Loop* l_bottem = solid->l_list[0];
	Vertex* tv = solid->v_list[0];
	HalfEdge* AB = mev(tv, pB, l_bottem);
	HalfEdge* BC = mev(AB->ev, pC, l_bottem);
	HalfEdge* CD = mev(BC->ev, pD, l_bottem);
	Loop* l_mid = mef(CD->ev, AB->sv, l_bottem, true);
	HalfEdge* AA_ = mev(AB->sv, pA_, l_mid);
	HalfEdge* BB_ = mev(BC->sv, pB_, l_mid);
	HalfEdge* CC_ = mev(BC->ev, pC_, l_mid);
	HalfEdge* DD_ = mev(CD->ev, pD_, l_mid);
	l_mid = mef(DD_->ev, AA_->ev, l_mid, false);
	l_mid = mef(AA_->ev, BB_->ev, l_mid, false);
	l_mid = mef(BB_->ev, CC_->ev, l_mid, false);
	l_mid = mef(CC_->ev, DD_->ev, l_mid, false);
	HalfEdge* the = AB->brother->lp->halfedges;
	HalfEdge* AE = mev(AB->sv, pE, AB->brother->lp);//deadloop
	Vertex* E = AE->ev;
	Loop* inlp = kemr(AE->sv, AE->ev, AB->brother->lp);
	HalfEdge* EF = mev(E, pF, inlp);
	HalfEdge* FG = mev(EF->ev, pG, inlp);
	HalfEdge* GH = mev(FG->ev, pH, inlp);
	Loop* in_mid = mef(GH->ev, EF->sv, l_bottem, false);
	HalfEdge* EE_ = mev(EF->sv, pE_, in_mid);
	HalfEdge* FF_ = mev(FG->sv, pF_, in_mid);
	HalfEdge* GG_ = mev(FG->ev, pG_, in_mid);
	HalfEdge* HH_ = mev(GH->ev, pH_, in_mid);
	in_mid = mef(HH_->ev, EE_->ev, in_mid, false);
	in_mid = mef(EE_->ev, FF_->ev, in_mid, false);
	in_mid = mef(FF_->ev, GG_->ev, in_mid, false);
	in_mid = mef(GG_->ev, HH_->ev, in_mid, false);
	kfmrh(l_mid->face, in_mid->face);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			bdown = true;
			xp = x;
			yp = y;
		}
		else
		{
			bdown = false;
			xrot += xrotr;
			yrot += yrotr;
		}
	}
}
void move(int x, int y)
{
	int deltx, delty;
	if (bdown)
	{
		deltx = x - xp;
		delty = y - yp;
		yrotr = ((double)deltx / 10.0 * 180.0 / PI * 0.04);
		xrotr = ((double)delty / 10.0 * 180.0 / PI * 0.04);
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(600, 500);
	glutCreateWindow("cad");
	creatSolid();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	obj = gluNewTess();
	gluTessCallback(obj, GLU_TESS_BEGIN, (void (CALLBACK*)())glBegin);
	gluTessCallback(obj, GLU_TESS_END, (void (CALLBACK*)())glEnd);
	gluTessCallback(obj, GLU_TESS_VERTEX, (void (CALLBACK*)())glVertex4dv);
	glutDisplayFunc(Draw);
	glutReshapeFunc(Reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(move);
	glutMainLoop();

	return 0;

}
