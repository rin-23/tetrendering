#pragma once

#include "RAEnginePrerequisites.h"
#include <Eigen/Core>
#include "RAES2TriMesh.h"
#include "RAES2Polyline.h"

namespace tyro
{
class IGLMeshWireframe : public ES2Polyline
{
public:
    IGLMeshWireframe() {mClip = Eigen::Vector3f(0,0,0);}

    virtual ~IGLMeshWireframe() {}
    
    void setClip(const Eigen::Vector3f& clip) { mClip = clip;} ;

    //Generate wireframe from unique edges
    static IGLMeshWireframeSPtr Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC);
    // static IGLMeshWireframeSPtr Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC, const std::vector<int>& eid_list);
    
    //static IGLMeshWireframeSPtr Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::Vector3d& color);
    void UpdateData(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC);

    virtual void UpdateUniformsWithCamera(const Camera* camera) override;

    ES2VisualEffectSPtr VisualEffect();

protected:
    void Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC);
    // void Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC, const std::vector<int>& eid_list);

    Eigen::Vector3f mClip;
private:

};
}