#pragma once
#ifndef _HE_H
#define _HE_H

#include <stdlib.h>
#include <vector>

using namespace std;
class Solid;
class Face;
class Loop;
class HalfEdge;
class Vertex;
class Edge;


class HalfEdge
{
public:
	Edge* edge;
	Vertex* sv;
	Vertex* ev;
	Loop* lp;
	HalfEdge* next;
	HalfEdge* pre;
	HalfEdge* brother;
	HalfEdge() : edge(NULL), sv(NULL), ev(NULL), lp(NULL), next(NULL), pre(NULL), brother(NULL) {}
};

class Edge
{
public:
	HalfEdge* half_l;
	HalfEdge* half_r;
	Edge* next;
	Edge* pre;
	Edge() : half_l(NULL), half_r(NULL), next(NULL), pre(NULL) {}
	void creatHalfEdge(Vertex* sv, Vertex* ev)
	{
		HalfEdge* half_l = new HalfEdge;
		HalfEdge* half_r = new HalfEdge;
		half_l->sv = sv;
		half_l->ev = ev;
		half_l->edge = this;
		half_l->brother = half_r;

		half_r->sv = ev;
		half_r->ev = sv;
		half_r->edge = this;
		half_r->brother = half_l;

		this->half_l = half_l;
		this->half_r = half_r;
	}
};


class Vertex
{
public:
	int id;
	double coordinate[3];
	Vertex* next;
	Vertex* pre;

	Vertex(double x, double y, double z) : id(0), next(NULL), pre(NULL)
	{
		coordinate[0] = x;
		coordinate[1] = y;
		coordinate[2] = z;
	}
};
class Loop
{
public:
	int id;
	HalfEdge* halfedges;
	Face* face; 
	Loop* next;
	Loop* pre;
	Loop() : id(0), halfedges(NULL), face(NULL), next(NULL), pre(NULL) {}
	void addEdge(Edge* edge)
	{
		HalfEdge* te = this->halfedges;
		HalfEdge* half_l = edge->half_l, * half_r = edge->half_r;
		half_l->lp = this;
		half_r->lp = this;
		if (this->halfedges == NULL)
		{
			this->halfedges = half_l;
			half_l->next = half_r;
			half_l->pre = half_r;

			half_r->next = half_l;
			half_r->pre = half_l;
		}
		else
		{
			HalfEdge* thalf = this->halfedges;
			while (thalf->ev != half_l->sv) thalf = thalf->next;
			half_l->pre = thalf;
			half_l->next = half_r;
			half_r->pre = half_l;
			half_r->next = thalf->next;
			thalf->next->pre = half_r;
			thalf->next = half_l;
		}
	}
};

class Face
{
public:
	int id;
	Solid* solid;
	Loop* out_lp;
	Loop* inner_lp;
	Face* next;
	Face* pre;
	int innum;
	Face() : id(0), solid(NULL), out_lp(NULL), next(NULL), pre(NULL), inner_lp(NULL), innum(0) {}
	void addLoop(Loop* loop)
	{
		loop->face = this;
		if (this->out_lp == NULL)
		{
			this->out_lp = loop;
		}
		else
		{
			Loop* tlp = this->inner_lp;
			if (tlp == NULL)
			{
				this->inner_lp = loop;
			}
			else
			{
				while (tlp->next != NULL)tlp = tlp->next;
				tlp->next = loop;
				loop->pre = tlp;
			}
			this->innum += 1;
		}
	}
};
class Solid
{
public:
	int id;
	Face* faces; 
	Edge* edges; 
	Solid* next;
	Solid* pre;
	
	vector<Vertex*> v_list;
	vector<Loop*> l_list;
	vector<Face*> sweep_list;

	int vnum;
	int fnum;
	int lnum;
	Solid() :id(0), faces(NULL), edges(NULL), next(NULL), pre(NULL), fnum(0), vnum(0), lnum(0)
	{
		v_list.clear();
		l_list.clear();
	}
	void addVertex(Vertex* v)
	{
		v->id = this->vnum;
		this->vnum += 1;
		this->v_list.push_back(v);
	}
	void addLoop(Loop* loop)
	{
		loop->id = this->vnum;
		this->lnum += 1;
		this->l_list.push_back(loop);
	}
	void addEdge(Edge* edge)
	{
		Edge* te = this->edges;

		if (te == NULL)this->edges = edge;
		else {
			while (te->next != NULL)te = te->next;
			te->next = edge;
			edge->pre = te;
		}
	}
	void addFace(Face* face)
	{
		Face* tface = this->faces;
		if (tface == NULL)
		{
			this->faces = face;
		}
		else
		{
			while (tface->next != NULL)tface = tface->next;
			tface->next = face;
			face->pre = tface;
		}
		face->solid = this;
		face->id = this->fnum;
		this->fnum += 1;
	}
};
Solid* mvfs(double point[3])
{
	/* 生成含有一个点的面，包含一个空环，而且构成一个新的体 */
	Solid* solid = new Solid();
	Face* face = new Face();
	Loop* loop = new Loop();
	Vertex* vertex = new Vertex(point[0], point[1], point[2]);
	
	solid->addLoop(loop);
	solid->addVertex(vertex);
	solid->addFace(face);
	face->addLoop(loop);

	return solid;
}
HalfEdge* mev(Vertex* sv, double point[3], Loop* loop)
{
	/*生成一个新的点v2，连接该点到已有点v1。构造一条新的边,返回这条半边*/
	Vertex* ev = new Vertex(point[0], point[1], point[2]);
	Edge* edge = new Edge();
	Solid* solid = loop->face->solid;

	solid->addVertex(ev);
	edge->creatHalfEdge(sv, ev);
	loop->addEdge(edge);
	solid->addEdge(edge);

	return edge->half_l;
}
Loop* mef(Vertex* sv, Vertex* ev, Loop* loop, bool mark)
{
	/*连接面f1上的两个点v1，v2，生成一条新边e，并产生一个新环和新面*/
	Solid* solid = loop->face->solid;
	Edge* edge = new Edge();
	Loop* newLoop = new Loop();
	edge->creatHalfEdge(sv, ev);
	
	HalfEdge* thalf = loop->halfedges;
	HalfEdge* tmpa, * tmpb, * tmpc;
	while (thalf->ev != sv)thalf = thalf->next;
	tmpa = thalf;

	while (thalf->ev != ev)thalf = thalf->next;
	tmpb = thalf;

	thalf = thalf->next;
	while (thalf->ev != ev)thalf = thalf->next;
	tmpc = thalf;

	HalfEdge* half_r = edge->half_r;
	HalfEdge* half_l = edge->half_l;
	half_r->next = tmpa->next;
	tmpa->next->pre = half_r;
	tmpa->next = half_l;
	half_l->pre = tmpa;

	half_l->next = tmpb->next;
	tmpb->next->pre = half_l;
	tmpb->next = half_r;
	half_r->pre = tmpb;
	loop->halfedges = half_l;
	newLoop->halfedges = half_r;
	half_l->lp = loop;
	half_r->lp = newLoop;

	Face* face = new Face();

	newLoop->id = solid->lnum;
	solid->lnum += 1;
	solid->l_list.push_back(newLoop);
	solid->addFace(face);
	face->addLoop(newLoop);

	if (tmpc == tmpb)
	{
		if (mark) 
		{
			solid->sweep_list.push_back(half_l->lp->face);
		}
	}
	else
	{
		solid->sweep_list.push_back(half_r->lp->face);
	}

	solid->addEdge(edge);

	return loop;
}

Loop* kemr(Vertex* sv, Vertex* ev, Loop* loop)
{
	/*删除一条边e。生成该边某一邻面上的新的内环*/
	HalfEdge* tmpa, * tmpb, * hal;
	Face* face = loop->face;
	Loop* inlp = new Loop();
	Solid* solid = loop->face->solid;

	hal = loop->halfedges;

	while (hal->sv != sv || hal->ev != ev)hal = hal->next;
	tmpa = hal;

	while (hal->sv != ev || hal->ev != sv)hal = hal->next;
	tmpb = hal;

	tmpb->pre->next = tmpa->next;
	tmpa->pre->next = tmpb->next;

	loop->face->solid->faces->out_lp->halfedges = tmpa->pre;

	inlp->halfedges = tmpb->pre;
	tmpb->pre->lp = inlp;

	inlp->id = solid->lnum;
	solid->lnum += 1;
	solid->l_list.push_back(inlp);

	tmpa->pre->brother->lp->face->addLoop(inlp);

	delete tmpa;
	delete tmpb;

	return NULL;
}

void kfmrh(Face* fa, Face* fb)
{
	Loop* loop = fb->out_lp;
	fa->addLoop(loop);
	
	Solid* solid = fa->solid;
	solid->lnum -= 1;
	solid->fnum -= 1;
	Face* face = solid->faces;
	if (face == fb)
	{
		solid->faces = face->next;
	}
	else
	{
		Face* tf = face;
		while (face != fb && face != NULL)
		{
			tf = face;
			face = face->next;
		}
		tf->next = face->next;
	}
	delete fb;
}
#endif // !_HE_H