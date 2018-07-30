#include <scrimmage/viewer/OriginAxes.h>

namespace scrimmage {

void OriginAxes::create(int length, vtkSmartPointer<vtkRenderer> &renderer)
{
    renderer_ = renderer;

    axes_ =
            vtkSmartPointer<vtkAxesActor>::New();
    axes_->SetTotalLength(length, length, length);

    // The axes are positioned with a user transform
    vtkSmartPointer<vtkTransform> transform =
            vtkSmartPointer<vtkTransform>::New();
    transform->Translate(0, 0.0, 0);

    axes_->SetUserTransform(transform);
    // properties of the axes_ labels can be set as follows
    // this sets the x axis label to red
    // axes_->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1,0,0);
    // the actual text of the axis label can be changed:
    // axes_->SetXAxisLabelText("test");
    renderer_->AddActor(axes_);
}

void OriginAxes::remove()
{
    renderer_->RemoveActor(axes_);
}

}
