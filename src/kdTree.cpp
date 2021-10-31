
#include "kdTree.h"
#include <algorithm>
#include "triangle.h"
#include "box.h"
#include <omp.h>
//#include <execution>

void KDTreeNode::Traverse(Ray &r) const{
    std::pair<real_number, real_number> t = mBounds.Intersect(r);
    return Traverse(r, t.first, t.second);
}

void KDTreeNode::Traverse(Ray &ray, real_number t_min, real_number t_max) const {
    if(RAY_IT_ECHO_LEVEL > 1) {
        std::cout<<"Triangles of this KDTreeNode:";
        for (auto triangle: mTriangles) {
            std::cout<<" "<<triangle->mId;
            triangle->Intersect(ray);
        }
        std::cout<<std::endl;
    }

    if (mLeaf) {
        for (auto triangle: mTriangles) {
            //std::cout<<"Trying intersection with triangle: "<<triangle->mId<<std::endl;
            triangle->Intersect(ray);
        }
    }
    else{
        real_number t_split;

        real_number aux;
        if (ray.mDirection[mSplitPlane.mAxis] == 0) {
            aux = INFINITY;
        } else {
            aux = ray.mInvDirection[mSplitPlane.mAxis];
        }
        t_split = (mSplitPlane.mPos - ray.mStartPosition[mSplitPlane.mAxis]) * aux;

        //t_split = (mSplitPlane.mPos - ray.mStartPosition[mSplitPlane.mAxis]) * (ray.mDirection[mSplitPlane.mAxis] == 0 ? INFINITY : ray.mInvDirection[mSplitPlane.mAxis]);

        // near is the side containing the origin of the ray
        KDTreeNode *near, *far;
        if(t_split == 0.0) {
            if (ray.mDirection[mSplitPlane.mAxis] > 0.0) {
                near = mLeft;
                far = mRight;
            } else {
                near = mRight;
                far = mLeft;
            }
        }
        else {
            if(ray.mStartPosition[mSplitPlane.mAxis] < mSplitPlane.mPos) {
                near = mLeft;
                far = mRight;
            } else {
                near = mRight;
                far = mLeft;
            }
        }

        if( t_split > t_max + EPSILON || t_split < 0) {
            near->Traverse(ray, t_min, t_max);
        }
        else if(t_split < t_min - EPSILON) {
            far->Traverse(ray, t_min, t_max);
        }
        else {
            near->Traverse(ray, t_min, t_split);
            if(ray.t_max < t_split)
                return;
            far->Traverse(ray, t_split, t_max);
        }
    }
}


#define COST_TRAVERSE real_number(1.0)
#define COST_INTERSECT real_number(1.5)

void KDTreeNode::splitBox(const Box& V, const SplitPlane& p, Box& VL, Box& VR) const {
    VL = V;
    VR = V;
    VL.mMax[p.mAxis] = p.mPos;
    VR.mMin[p.mAxis] = p.mPos;
}

// surface area of a volume V
inline real_number surfaceArea(const Box& V) {
    return 2*V.XWidth()*V.YWidth() + 2*V.XWidth()*V.ZWidth() + 2*V.YWidth()*V.ZWidth();
}

// Probability of hitting volume Vsub, given volume V was hit
real_number prob_hit(const Box& Vsub, const Box& V){
    return surfaceArea(Vsub) / surfaceArea(V);
}

// bias for the cost function s.t. it is reduced if NL or NR becomes zero
inline real_number lambda(size_t NL, size_t NR, real_number PL, real_number PR) {
    if((NL == 0 || NR == 0) &&
       !(PL == 1 || PR == 1) // NOT IN PAPER
       )
        return real_number(0.8);
    return real_number(1.0);
}

inline real_number cost(real_number PL, real_number PR, size_t NL, size_t NR) {
    return(lambda(NL, NR, PL, PR) * (COST_TRAVERSE + COST_INTERSECT * (PL * NL + PR * NR)));
}

// SAH heuristic for computing the cost of splitting a voxel V using a plane p
void KDTreeNode::SAH(const SplitPlane& p, const Box& V, size_t NL, size_t NR, size_t NP, real_number& CP, PlaneSide& pside) const {
    CP = INFINITY;
    Box VL, VR;
    splitBox(V, p, VL, VR);
    real_number PL, PR;
    PL = prob_hit(VL, V);
    PR = prob_hit(VR, V);
    if(PL == 0 || PR == 0) // NOT IN PAPER
        return;
    if(V.WidthAlongAxis(p.mAxis) == 0) // NOT IN PAPER
        return;
    real_number CPL, CPR;
    CPL = cost(PL, PR, NL + NP, NR);
    CPR = cost(PL, PR, NL, NP + NR );
    if(CPL < CPR) {
        CP = CPL;
        pside = LEFT;
    } else {
        CP = CPR;
        pside = RIGHT;
    }
}

// criterion for stopping subdividing a tree node
inline bool KDTreeNode::isDone(size_t N, real_number minCv) const {
    // cerr << "terminate: minCv=" << minCv << ", KI*N=" << KI*N << endl;
    return(minCv > COST_INTERSECT*N);
}


// get primitives's clipped bounding box
Box clipTriangleToBox(Triangle* t, const Box& V) {
    Box b = t->mBoundingBox;
    for(int k=0; k<3; k++) {
        if(V.mMin[k] > b.mMin[k])
            b.mMin[k] = V.mMin[k];
        if(V.mMax[k] < b.mMax[k])
            b.mMax[k] = V.mMax[k];
    }
    return b;
}

struct Event {
    typedef enum { endingOnPlane=0, lyingOnPlane=1, startingOnPlane=2  } EventType;
    Triangle* triangle;
    SplitPlane splitPlane;
    EventType type;

    Event(Triangle* tri, int k, real_number ee0, EventType type) : triangle(tri), type(type) , splitPlane(SplitPlane(k, ee0)){}

    inline bool operator<(const Event& e) const {
        return((splitPlane.mPos < e.splitPlane.mPos) || (splitPlane.mPos == e.splitPlane.mPos && type < e.type));
    }


};

// best spliting plane using SAH heuristic
void KDTreeNode::findPlane(const std::vector<Triangle *>& T, const Box& V,
               SplitPlane& p_est, real_number& C_est, PlaneSide& pside_est) const {
    // static int count = 0;
    C_est = INFINITY;
    for(int k=0; k<3; ++k) {
        std::vector<Event> events;
        events.reserve(T.size()*2);
        for(size_t i=0; i<T.size(); i++) {
            Box B = clipTriangleToBox(T[i], V);
            if(B.isPlanar()) {
                events.push_back(Event(T[i], k, B.mMin[k], Event::lyingOnPlane));
            } else {
                events.push_back(Event(T[i], k, B.mMin[k], Event::startingOnPlane));
                events.push_back(Event(T[i], k, B.mMax[k], Event::endingOnPlane));
            }
        }
        //sort(std::execution::par_unseq, events.begin(), events.end());
        sort(events.begin(), events.end());
        size_t NL = 0, NP = 0, NR = T.size();
        for(std::vector<Event>::size_type Ei = 0; Ei < events.size(); ++Ei) {
            const SplitPlane& p = events[Ei].splitPlane;
            int pLyingOnPlane = 0, pStartingOnPlane = 0, pEndingOnPlane = 0;
            while(Ei < events.size() && events[Ei].splitPlane.mPos == p.mPos && events[Ei].type == Event::endingOnPlane) {
                ++pEndingOnPlane;
                Ei++;
            }
            while(Ei < events.size() && events[Ei].splitPlane.mPos == p.mPos && events[Ei].type == Event::lyingOnPlane) {
                ++pLyingOnPlane;
                Ei++;
            }
            while(Ei < events.size() && events[Ei].splitPlane.mPos == p.mPos && events[Ei].type == Event::startingOnPlane) {
                ++pStartingOnPlane;
                Ei++;
            }
            NP = pLyingOnPlane;
            NR -= pLyingOnPlane;
            NR -= pEndingOnPlane;
            real_number C;
            PlaneSide pside = UNKNOWN;
            SAH(p, V, NL, NR, NP, C, pside);
            if(C < C_est) {
                C_est = C;
                p_est = p;
                pside_est = pside;
            }
            NL += pStartingOnPlane;
            NL += pLyingOnPlane;
            NP = 0;
        }
    }
}


void KDTreeNode::DistributeTriangles(const std::vector<Triangle*>& T, const SplitPlane& p, const PlaneSide& pside, std::vector<Triangle*>& TL, std::vector<Triangle*>& TR) const {
    /*const int num_threads = omp_get_max_threads();
    std::vector<std::vector<Triangle*>> thread_TL; thread_TL.resize(num_threads);
    std::vector<std::vector<Triangle*>> thread_TR; thread_TR.resize(num_threads);
    #pragma omp parallel for if(T.size()>1000)*/
    for(int i=0; i<(int)T.size(); i++) {
        Triangle* t = T[i];
        Box tbox = t->mBoundingBox;
        //const int this_thread = omp_get_thread_num();
        if(tbox.mMin[p.mAxis] == p.mPos && tbox.mMax[p.mAxis] == p.mPos) {
            if(pside == LEFT)
                TL.push_back(t);
            else if(pside == RIGHT)
                TR.push_back(t);
            else
                std::cout << "ERROR WHILE SORTING TRIANGLES" << std::endl;
        } else {
            if(tbox.mMin[p.mAxis] < p.mPos)
                TL.push_back(t);
            if(tbox.mMax[p.mAxis] > p.mPos)
                TR.push_back(t);
        }
    }
    /*for(int i=0; i<num_threads;i++){
        TL.insert(TL.end(), thread_TL[i].begin(), thread_TL[i].end());
        TR.insert(TR.end(), thread_TR[i].begin(), thread_TR[i].end());
    }*/
}

int nnodes = 0;
KDTreeNode* KDTreeNode::RecursiveTreeNodeBuild(const std::vector<Triangle *>& triangles, const Box &V, int depth, const SplitPlane& prev_plane){
    SplitPlane p;
    real_number Cp;
    PlaneSide pside;
    findPlane(triangles, V, p, Cp, pside);
    if(isDone(triangles.size(), Cp) || p == prev_plane) // NOT IN PAPER
    {
        // Leaf node
        KDTreeNode* leafnode = new KDTreeNode();
        leafnode->mTriangles = triangles;
        leafnode->mBounds = V;
        leafnode->mLeaf = true;
        //std::cout << "Leaf node. Depth: " << depth << ". Number of triangles: "<< triangles.size() << std::endl;
        return leafnode;
    }



    Box VL, VR;
    splitBox(V, p, VL, VR); // TODO: avoid doing this step twice
    std::vector<Triangle *> TL, TR;
    DistributeTriangles(triangles, p, pside, TL, TR);
    // Inner node
    KDTreeNode * innerNode = new KDTreeNode();

    ////TODO: remove
    /* innerNode->mTriangles = triangles;
    innerNode->mBounds = V;
    innerNode->mLeaf = false; */
    ////

    innerNode->mSplitPlane = p;
    innerNode->mBounds = V;
    innerNode->mLeaf = false;
    innerNode->mLeft = RecursiveTreeNodeBuild(TL, VL, depth+1, p);
    innerNode->mRight = RecursiveTreeNodeBuild(TR, VR, depth+1, p);

    if(RAY_IT_ECHO_LEVEL > 1) {
        std::cout<<"Depth "<<depth<<" Splitting axis: "<<p.mAxis<<"  split pos: "<<p.mPos<<std::endl;
        std::cout<<"Triangles Left:";
        for(Triangle* tri : TL){
            std::cout<<" "<<tri->mId;
        }
        std::cout<<" "<<std::endl;
        std::cout<<"Triangles Right:";
        for(Triangle* tri : TR){
            std::cout<<" "<<tri->mId;
        }
        std::cout<<" "<<std::endl;
    }
    return innerNode;
}











