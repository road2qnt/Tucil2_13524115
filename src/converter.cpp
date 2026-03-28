#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>
using namespace std;


#define VF  vector<Face>
#define VV  vector<Vec3>
#define VB  vector<Box>
#define VI  vector<int>
#define SS  stringstream
#define CLK chrono::high_resolution_clock
#define DUR chrono::duration<double>

#define DOT(a,b)  ((a).x*(b).x + (a).y*(b).y + (a).z*(b).z)
#define CROSS(a,b) Vec3{(a).y*(b).z-(a).z*(b).y, (a).z*(b).x-(a).x*(b).z, (a).x*(b).y-(a).y*(b).x}
#define FMIN3(a,b,c) min({(a),(b),(c)})
#define FMAX3(a,b,c) max({(a),(b),(c)})

#define REP(i,n)   for(int i=0;i<(n);i++)
#define EACH(x,v)  for(auto& x:(v))

struct Vec3 {
    float x,y,z;
    Vec3 operator-(const Vec3& o) const { return {x-o.x,y-o.y,z-o.z}; }
    Vec3 operator+(const Vec3& o) const { return {x+o.x,y+o.y,z+o.z}; }
    Vec3 operator*(float s)       const { return {x*s,y*s,z*s}; }
};

struct Face { Vec3 v0,v1,v2; };

struct Box {
    Vec3 mn, mx;
    Vec3 ctr()  const { return (mn+mx)*0.5f; }
    Vec3 sz()   const { return mx-mn; }
    Vec3 corner(int i) const {
        return {(i&1)?mx.x:mn.x, (i&2)?mx.y:mn.y, (i&4)?mx.z:mn.z};
    }
    Box oct(int i) const {
        Vec3 c=ctr();
        return {
            {(i&1)?c.x:mn.x, (i&2)?c.y:mn.y, (i&4)?c.z:mn.z},
            {(i&1)?mx.x:c.x, (i&2)?mx.y:c.y, (i&4)?mx.z:c.z}
        };
    }
};

bool hit(const Box& b, const Face& f) {
    if(FMAX3(f.v0.x,f.v1.x,f.v2.x)<b.mn.x || FMIN3(f.v0.x,f.v1.x,f.v2.x)>b.mx.x) return false;
    if(FMAX3(f.v0.y,f.v1.y,f.v2.y)<b.mn.y || FMIN3(f.v0.y,f.v1.y,f.v2.y)>b.mx.y) return false;
    if(FMAX3(f.v0.z,f.v1.z,f.v2.z)<b.mn.z || FMIN3(f.v0.z,f.v1.z,f.v2.z)>b.mx.z) return false;

    Vec3 e1=f.v1-f.v0, e2=f.v2-f.v0, n=CROSS(e1,e2);
    int s1=0,s2=0;
    REP(i,8){ float d=DOT(n, b.corner(i)-f.v0); if(d>0)s1++; else if(d<0)s2++; }
    if(s1==8||s2==8) return false;
    return true;
}

VB    vox;
VI    nc, np;  
int   mdp;     

void voxelize(const Box& cur, int dep, const VF& af) {
    nc[dep]++;
    VF iF;
    EACH(f,af) if(hit(cur,f)) iF.push_back(f);
    if(iF.empty()){ np[dep]++; return; }
    if(dep==mdp){ vox.push_back(cur); return; }
    REP(i,8) voxelize(cur.oct(i), dep+1, iF);
}

int main(int argc, char** argv) {
    if(argc<3){ cerr<<"usage: ./converter <input.obj> <max_depth>\n"; return 1; }

    string ip=argv[1];
    mdp=stoi(argv[2]);
    nc.assign(mdp+1,0);
    np.assign(mdp+1,0);

    VV verts;
    VF faces;
    ifstream in(ip);
    if(!in){ cerr<<"[ERROR] gagal buka file obj!\n"; return 1; }

    string ln;
    Box root={{1e9f,1e9f,1e9f},{-1e9f,-1e9f,-1e9f}};

    while(getline(in,ln)){
        SS ss(ln); string t; ss>>t;
        if(t=="v"){
            Vec3 v; ss>>v.x>>v.y>>v.z;
            verts.push_back(v);
            root.mn.x=min(root.mn.x,v.x); root.mx.x=max(root.mx.x,v.x);
            root.mn.y=min(root.mn.y,v.y); root.mx.y=max(root.mx.y,v.y);
            root.mn.z=min(root.mn.z,v.z); root.mx.z=max(root.mx.z,v.z);
        } else if(t=="f"){
            int i,j,k; ss>>i>>j>>k;
            faces.push_back({verts[i-1],verts[j-1],verts[k-1]});
        }
    }

    auto t0=CLK::now();
    voxelize(root,1,faces);
    DUR dt=CLK::now()-t0;

    string op=ip.substr(0,ip.find_last_of('.'))+"-voxelized.obj";
    ofstream out(op);

    int vo=1;
    EACH(v,vox){
        REP(i,8){ Vec3 c=v.corner(i); out<<"v "<<c.x<<" "<<c.y<<" "<<c.z<<"\n"; }
        int f[6][4]={{1,5,7,3},{4,8,6,2},{1,2,6,5},{3,7,8,4},{1,3,4,2},{5,6,8,7}};
        REP(i,6) out<<"f "<<f[i][0]+vo-1<<" "<<f[i][1]+vo-1<<" "<<f[i][2]+vo-1<<" "<<f[i][3]+vo-1<<"\n";
        vo+=8;
    }

    cout<<"====================================\n"
        <<"voxel : "<<vox.size()<<"\n"
        <<"vertex: "<<vox.size()*8<<"\n"
        <<"faces : "<<vox.size()*6<<"\n"
        <<"depth : "<<mdp<<"\n"
        <<"time  : "<<dt.count()<<" s\n"
        <<"output: "<<op<<"\n"
        <<"------------------------------------\n"
        <<"node terbentuk:\n";
    for(int i=1;i<=mdp;i++) cout<<i<<": "<<nc[i]<<"\n";
    cout<<"------------------------------------\n"
        <<"node tidak ditelusuri:\n";
    for(int i=1;i<=mdp;i++) cout<<i<<": "<<np[i]<<"\n";
    cout<<"====================================\n";
    return 0;
}