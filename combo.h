#ifndef COMBO_H
#define COMBO_H
#include "generalshape.h"
#include <Qpainter>
#include <QtXml>
#include "curve.h"
#include "closecurve.h"
#include "circle.h"
#include "Polyline.h"
#include "Polygon.h"
#include "Rect.h"
#include "Ellipse.h"
#include "text.h"

class Combo : public GeneralShape
{
public:
    Combo();
    virtual Combo *  copyPaste(){
        Combo* tmp=new Combo;
        foreach (GeneralShape* sp, shapes) {
            tmp->shapes.append(sp->copyPaste());
            tmp->shapes.last()->updateRange();
        }
        tmp->points=points;
        tmp->name=name;
        tmp->pen=pen;
        tmp->Rotationangle=Rotationangle;
        tmp->sx=sx;
        tmp->sy=sy;
        tmp->brush=brush;
        tmp->updateRange();//这一步不能少！！！！！！
        return tmp;
    }

    void setShapes(QDomElement shapesElement){

        QDomNode node=shapesElement.firstChild(); //获得第一个子节点
        while(!node.isNull())  //如果节点不空
        {
            if(node.isElement()) //如果节点是元素
            {
                QDomElement e=node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多

                GeneralShape* currentShape;



                QDomNode n=e.namedItem("name");

                currentShape=NULL;
                if (n.toElement().text()=="Curve"){
                    currentShape = new Curve;
                }
                if (n.toElement().text()=="CloseCurve"){
                    currentShape = new CloseCurve;
                }
                if (n.toElement().text()=="Circle"){
                    currentShape = new Circle;
                }
                if (n.toElement().text()=="Ellipse"){
                    currentShape = new Ellipse;
                }
                if (n.toElement().text()=="Polygon"){
                    currentShape = new Polygon;
                }
                if (n.toElement().text()=="Polyline"){
                    currentShape = new Polyline;
                }
                if (n.toElement().text()=="Rect"){
                    currentShape = new Rect;
                }
                if (n.toElement().text()=="Text"){
                    currentShape = new Text;
                }
                if (n.toElement().text()=="Combo"){
                    currentShape = new Combo;
                }
                if (currentShape==NULL) currentShape=new Curve;


                //if (n.nodeName()=="points")

                n=e.namedItem("points");
                QString line=n.toElement().text();

                currentShape->fromline(line);

                //if (n.nodeName()=="Rotationangle")

                n=e.namedItem("Rotationangle");

                currentShape->Rotationangle=n.toElement().text().toDouble();


                //if (n.nodeName()=="mytext")

                n=e.namedItem("mytext");
                Text *pm = dynamic_cast< Text* >( currentShape );
                if ( pm ){
                    pm->setText(n.toElement().text());

                }


                //if (n.nodeName()=="myfont")

                n=e.namedItem("myfont");
                pm = dynamic_cast< Text* >( currentShape );
                if ( pm ){
                    QFont font;
                    font.setPointSize(n.toElement().attribute("pointSize").toInt());
                    pm->setFont(font);
                }


                //if (n.nodeName()=="shapes")

                n=e.namedItem("shapes");

                Combo *cm = dynamic_cast< Combo* >( currentShape );
                if ( cm ){
                    cm->setShapes(n.toElement());

                }


                //if (n.nodeName()=="brush")

                n=e.namedItem("brush");

                int r,g,b,brushstyle;
                r=n.toElement().attribute("red").toInt();
                g=n.toElement().attribute("green").toInt();
                b=n.toElement().attribute("blue").toInt();

                brushstyle=n.toElement().attribute("brushstyle").toInt();
                Qt::BrushStyle castStyle = (Qt::BrushStyle)brushstyle;
                //currentShape->brush.setColor(QColor(r,g,b));//this code has no effect,why?
                //currentShape->setBrush(QColor(r,g,b),Qt::BrushStyle(brushstyle));
                QBrush brush=QBrush(QColor(r,g,b),castStyle);
                currentShape->setBrush(brush);



                currentShape->sx = e.attribute("sx").toDouble()!=0 ? e.attribute("sx").toDouble() : 1;
                currentShape->sy = e.attribute("sy").toDouble()!=0 ? e.attribute("sy").toDouble() : 1;
                //qDebug()<<currentShape->sx;
                currentShape->updateRange();
                shapes.append(currentShape);
            }
            node=node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
        }
        updateRange();
    }

    QDomElement toElement(){
        QDomDocument doc;
        QDomElement shapesElement=doc.createElement("shapes");

        doc.appendChild(shapesElement);




        foreach(GeneralShape *sp, shapes){
            QDomElement shape=doc.createElement("shape");
            shape.setAttribute("sx",sp->sx); //方式一：创建属性  其中键值对的值可以是各种类型
            shape.setAttribute("sy",sp->sy);

            QDomText text; //设置括号标签中间的值

            QDomElement name=doc.createElement("name"); //创建子元素
            text=doc.createTextNode(sp->name);
            shape.appendChild(name);
            name.appendChild(text);

            QDomElement points=doc.createElement("points"); //创建子元素
            text=doc.createTextNode(sp->qStringFromPoints());
            shape.appendChild(points);
            points.appendChild(text);

            QDomElement Rotationangle=doc.createElement("Rotationangle"); //创建子元素
            text=doc.createTextNode(QString::number(sp->rotationangle()) );
            shape.appendChild(Rotationangle);
            Rotationangle.appendChild(text);

            QDomElement brush=doc.createElement("brush"); //创建子元素
            brush.setAttribute("red",sp->brush.color().red());
            brush.setAttribute("green",sp->brush.color().green());
            brush.setAttribute("blue",sp->brush.color().blue());
            brush.setAttribute("brushstyle",sp->brush.style());
            shape.appendChild(brush);


            if (sp->name=="Text"){
                QDomElement mytext=doc.createElement("mytext"); //创建子元素
                QDomElement myfont=doc.createElement("myfont");

                Text *pm = dynamic_cast< Text* >( sp );
                if ( pm ){
                text=doc.createTextNode(pm->text());
                shape.appendChild(mytext);
                mytext.appendChild(text);

                myfont.setAttribute("pointSize",pm->myfont.pointSize());
                shape.appendChild(myfont);

                }

            }
            if (sp->name=="Combo"){
                Combo *pm = dynamic_cast< Combo* >( sp );
                if ( pm ){
                    QDomElement shapes=pm->toElement();
                shape.appendChild(shapes);
                }

            }

            shapesElement.appendChild(shape);
    //        out << shape->qStringFromThis();
    //        out<<endl;
        }
        return shapesElement;
    }



    QString virtual qStringFromThis(){
        return "";
    }

    QString virtual qStringFromPoints(){
        return "";
    }

    void virtual draw(QPainter &painter,qreal zoomRatio){
        double left=minx;
        double right=maxx;
        double top=miny;
        double bottom=maxy;

        painter.translate((left+right)/2*zoomRatio, (top+bottom)/2*zoomRatio);
        painter.rotate( Rotationangle );
        Combo* tmp=copyPaste();
        tmp->drag(QPointF(-(left+right)/2,-(top+bottom)/2));
        //zoom(sx);

        foreach (GeneralShape* sp, tmp->shapes) {
            //GeneralShape *tmp2=sp->copyPaste();
            sp->zoom(sx);
            sp->draw(painter,zoomRatio);
            //sp->times(1/sx);
        }
        //zoom(1/sx);
        //drag(QPointF((left+right)/2,(top+bottom)/2));
        painter.rotate( -Rotationangle );
        painter.translate(-((left+right)/2)*zoomRatio, -((top+bottom)/2)*zoomRatio);


    }
//    void virtual draw(QPainter &painter,qreal zoomRatio){
//        double left=minx;
//        double right=maxx;
//        double top=miny;
//        double bottom=maxy;

//        painter.translate((left+right)/2*zoomRatio, (top+bottom)/2*zoomRatio);
//        painter.rotate( Rotationangle );
//        //Combo* tmp=copyPaste();
//        drag(QPointF(-(left+right)/2,-(top+bottom)/2));
//        zoom(sx);

//        foreach (GeneralShape* sp, shapes) {

//            sp->draw(painter,zoomRatio);
//            //sp->times(1/sx);
//        }
//        zoom(1/sx);
//        drag(QPointF((left+right)/2,(top+bottom)/2));
//        painter.rotate( -Rotationangle );
//        painter.translate(-((left+right)/2)*zoomRatio, -((top+bottom)/2)*zoomRatio);


//    }


    void virtual zoom(qreal zoomratio){
        foreach (GeneralShape* sp, shapes) {
            sp->zoom(zoomratio);
        }

        updateRange();
    }

    double virtual minDistance(QPointF point){
        double left=minx;
        double right=maxx;
        double top=miny;
        double bottom=maxy;

        point=point-QPointF((left+right)/2,(top+bottom)/2);
        point=rotated(point,Rotationangle/180*M_PI);

        drag(QPointF(-(left+right)/2,-(top+bottom)/2));
        double minD=MAX;
        foreach (GeneralShape* sp, shapes) {
            GeneralShape* tmp=sp->copyPaste();
            tmp->zoom(sx);
            minD=min(minD,tmp->minDistance(point));
        }
        drag(QPointF((left+right)/2,(top+bottom)/2));


        return minD;
    }

    void virtual drawClosure(QPainter &painter,qreal zoomRatio){
        QPen pen;  // creates a default pen

        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        //QBrush defaultbrush=QBrush(QColor(ARGB 1, 0, 0, 0) , Qt::NoBrush );
        painter.setBrush( QBrush( Qt::NoBrush ));
        updateRange();
        double left=minx;
        double right=maxx;
        double top=miny;
        double bottom=maxy;
        painter.translate((left+right)/2*zoomRatio, (top+bottom)/2*zoomRatio);
        painter.rotate( Rotationangle );
        painter.drawRect((left-right)/2*zoomRatio*sx,(top-bottom)/2*zoomRatio*sy,
                         (right-left)*zoomRatio*sx,(bottom-top)*zoomRatio*sy);
        painter.drawLine(QPointF(0,(top-bottom)/2*zoomRatio*sy) ,
                         QPointF(0,(top-bottom)/2*zoomRatio*sy-sy/abs(sy)*lenthOfRotationHandleLine));
        painter.setPen(QPen(Qt::black,3));
        painter.drawPoint(-(left-right)/2*zoomRatio*sx,-(top-bottom)/2*zoomRatio*sy);
        painter.setPen(pen);
        painter.rotate( -Rotationangle );
        painter.translate(-((left+right)/2)*zoomRatio, -((top+bottom)/2)*zoomRatio);
    }

    QPointF virtual rotationHandlePoint(){
        double left=minx;
        double right=maxx;
        double top=miny;
        double bottom=maxy;
        //double x=0;
        double y=(top-bottom)/2*sy;
        double x1=-y*sin(Rotationangle/180*M_PI);
        double y1=y*cos(Rotationangle/180*M_PI);
        return QPointF(x1+(left+right)/2,y1+(top+bottom)/2);
    }

    QPointF virtual scaleHandlePoint(){
        double left=minx;
        double right=maxx;
        double top=miny;
        double bottom=maxy;
        double x=(left-right)/2*sx;
        double y=(top-bottom)/2*sy;
        double sita=Rotationangle/180*M_PI;
        double x1=x*cos(sita)-y*sin(sita);
        double y1=x*sin(sita)+y*cos(sita);
        return QPointF(-x1+(left+right)/2,-y1+(top+bottom)/2);
    }


    void virtual zoom(qreal zx,qreal zy){

        updateRange();
    }


    bool virtual inRange(QPointF p0,QPointF p1){

            double left=min(p0.x(),p1.x());
            double right=max(p0.x(),p1.x());
            double top=min(p0.y(),p1.y());
            double bottom=max(p0.y(),p1.y());
            if (minx>left && maxx<right && miny>top && maxy<bottom) return true;
            else return false;

    }
    void virtual updateRange() {
        if (shapes.size()==0) return;
        minx=shapes.at(0)->left;
        maxx=shapes.at(0)->right;
        miny=shapes.at(0)->top;
        maxy=shapes.at(0)->bottom;
        for (int i=1;i<shapes.size();i++){

            minx=min(shapes.at(i)->left,minx);
            miny=min(shapes.at(i)->top,miny);
            maxx=max(shapes.at(i)->right,maxx);
            maxy=max(shapes.at(i)->bottom,maxy);
        }
        updateBand();
    }

    void virtual drag(QPointF point)
    {
        for (int i=0;i<shapes.size();i++){
            shapes.at(i)->drag(point);
        }
        updateRange();
    }
    bool virtual isEmpty(){
        return false;
    }
    qreal rotationangle(){
        return Rotationangle;
    }
    void setRotationangle(qreal angle){
        Rotationangle=angle;
    }
    void virtual addPoint(QPointF point){

    }

    void virtual removeLastPoint(){

    }

    void virtual setPen(const QPen &pen){
        for (int i=0;i<shapes.size();i++){
            shapes.at(i)->setPen(pen);
        }
    }

    void virtual setBrush(const QBrush &brush){

    }
    void virtual setsx(double x){
        sx=x;
        sy=x;
    }
    void virtual setsy(double y){
        //do nothing
    }
    double virtual getsy(){
        return sx;
    }

//private:
    QList<GeneralShape*> shapes;
};

#endif // COMBO_H