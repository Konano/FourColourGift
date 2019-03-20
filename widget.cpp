#include "widget.h"
#include "ui_widget.h"

#include <QIntValidator>
#include <QTime>
#include <QTimer>
#include <qmath.h>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>

#define R 10

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    ui->lineEdit->setValidator(new QIntValidator(10,400,this));
}

Widget::~Widget()
{
    delete ui;
}

double Widget::dis(int a, int b) {
    return sqrt(pow(X[a]-X[b],2)+pow(Y[a]-Y[b],2));
}

void Widget::changeLabel()
{
    ui->label->setText(tr("最少颜色：")+colorName(C)+'\n'+
                       tr("当前笔刷颜色：")+colorName(Cnow)+'\n'+
                       tr("现在个数：")+QString::number(cnt[C])+'\n'+
                       tr("最优解：")+QString::number(best));
}

QString Widget::colorName(int c)
{
    if (c == 0) return tr("白");
    if (c == 1) return tr("红");
    if (c == 2) return tr("黄");
    if (c == 3) return tr("蓝");
    if (c == 4) return tr("绿");
    return tr("非法颜色");
}

Qt::GlobalColor Widget::colorQT(int c)
{
    if (c == 0) return Qt::white;
    if (c == 1) return Qt::red;
    if (c == 2) return Qt::yellow;
    if (c == 3) return Qt::blue;
    if (c == 4) return Qt::green;
    return Qt::black;
}

void Widget::link(int a, int b)
{
    l.push_back(a*num+b);
    v[a].push_back(b);
    v[b].push_back(a);
}

double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
    double cross = (x2 - x1)*(x - x1) + (y2 - y1)*(y - y1);
    if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
    double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
    double r = cross / d2;
    double px = x1 + r*(x2-x1);
    double py = y1 + r*(y2-y1);
    // qDebug() << sqrt((x - px) * (x - px) + (py - y) * (py - y));
    return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}

bool Widget::canLink(int a, int b)
{
    // qDebug() << a << b;
    for(int i=0; i<num; i++) if (i!=a && i!=b && PointToSegDist(X[i],Y[i],X[a],Y[a],X[b],Y[b])<1.1*R) return false;
    return true;
}

bool Widget::inCircle(polygon poly, int point)
{
    int A1=2*(X[poly.b]-X[poly.a]);
    int B1=2*(Y[poly.b]-Y[poly.a]);
    long long C1=1LL*X[poly.b]*X[poly.b]+1LL*Y[poly.b]*Y[poly.b]-1LL*X[poly.a]*X[poly.a]-1LL*Y[poly.a]*Y[poly.a];
    int A2=2*(X[poly.c]-X[poly.b]);
    int B2=2*(Y[poly.c]-Y[poly.b]);
    long long C2=1LL*X[poly.c]*X[poly.c]+1LL*Y[poly.c]*Y[poly.c]-1LL*X[poly.b]*X[poly.b]-1LL*Y[poly.b]*Y[poly.b];
    double x=1.0*((1LL*C1*B2)-(1LL*C2*B1))/((1LL*A1*B2)-(1LL*A2*B1));
    double y=1.0*((1LL*A1*C2)-(1LL*A2*C1))/((1LL*A1*B2)-(1LL*A2*B1));
    double a=dis(poly.a,poly.b);
    double b=dis(poly.b,poly.c);
    double c=dis(poly.c,poly.a);
    double p=(a+b+c)/2;
    double s=sqrt(p*(p-a)*(p-b)*(p-c));
    double r=a*b*c/4/s;

    return sqrt(pow(x-X[point],2)+pow(y-Y[point],2))<r;
}

void Widget::on_pushButton_clicked()
{
    num = ui->lineEdit->text().toInt();
    for(int i=0; i<num; i++) {
        again:;
        X[i]=qrand()%1000+10;
        Y[i]=qrand()%800+10;
        for(int j=0; j<i; j++) if (dis(i,j) <= 3*R) goto again;
        qDebug() << i << X[i] << Y[i];
    }
    X[num+0]=X[num+1]=Y[num+0]=Y[num+2]=-10000;
    X[num+2]=X[num+3]=Y[num+1]=Y[num+3]=10000;
    p.clear();
    p.push_back((polygon){num+0,num+1,num+2});
    p.push_back((polygon){num+1,num+2,num+3});

    for(int i=0; i<num; i++) {
        e.clear();
        for(int j=0; j<(int)p.size(); j++)
            if (inCircle(p[j], i)) {
                if (e.find((edge){p[j].a,p[j].b})==e.end()) e.insert((edge){p[j].a,p[j].b}); else e.erase((edge){p[j].a,p[j].b});
                if (e.find((edge){p[j].b,p[j].c})==e.end()) e.insert((edge){p[j].b,p[j].c}); else e.erase((edge){p[j].b,p[j].c});
                if (e.find((edge){p[j].a,p[j].c})==e.end()) e.insert((edge){p[j].a,p[j].c}); else e.erase((edge){p[j].a,p[j].c});
                p[j--] = p[p.size()-1];
                p.pop_back();
            }
        // qDebug() << e.size();
        for(std::set<edge>::iterator iter = e.begin(); iter != e.end(); iter++) {
            if (i < (*iter).x) p.push_back((polygon){i,(*iter).x,(*iter).y});
            else if (i < (*iter).y) p.push_back((polygon){(*iter).x,i,(*iter).y});
            else p.push_back((polygon){(*iter).x,(*iter).y,i});
        }
        // qDebug() << p.size();
        // for(int j=0; j<(int)p.size(); j++) qDebug() << p[j].a << p[j].b << p[j].c;
    }

    e.clear();
    for(int j=0; j<(int)p.size(); j++) {
        if (p[j].b < num)
            e.insert((edge){p[j].a,p[j].b});
        if (p[j].c < num)
            e.insert((edge){p[j].b,p[j].c});
        if (p[j].c < num)
            e.insert((edge){p[j].a,p[j].c});
    }

    l.clear();
    for(int i=0; i<num; i++) v[i].clear();
    for(std::set<edge>::iterator iter = e.begin(); iter != e.end(); iter++)
        if (canLink((*iter).x, (*iter).y))
            link((*iter).x, (*iter).y);
    // qDebug() << l.size();

    gaming = true;
    for(int i=0; i<num; i++) col[i]=0;
    col[0] = qrand()%4+1; // 0 初始
    for(int i=1; i<=4; i++) cnt[i]=0;
    cnt[col[0]]++;
    C = qrand()%4+1;
    Cnow = 0;
    best = 999;
    changeLabel();
    repaint();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    QBrush bruch(Qt::SolidPattern);
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);

    for(std::vector<int>::iterator iter = l.begin(); iter != l.end(); iter++) {
        if (col[(*iter)%num]==col[(*iter)/num] && col[(*iter)%num])
            pen.setWidth(3);
        else
            pen.setWidth(1);
        painter.setPen(pen);
        painter.drawLine(X[(*iter)%num],Y[(*iter)%num],X[(*iter)/num],Y[(*iter)/num]);
    }

    pen.setWidth(3);
    painter.setPen(pen);
    bruch.setColor(colorQT(col[0]));
    painter.setBrush(bruch);
    painter.drawEllipse(X[0]-R,Y[0]-R,2*R,2*R);
    pen.setWidth(1);
    painter.setPen(pen);
    for(int i=1; i<num; i++) {
        bruch.setColor(colorQT(col[i]));
        painter.setBrush(bruch);
        painter.drawEllipse(X[i]-R,Y[i]-R,2*R,2*R);
    }
}

void Widget::on_pushButton_3_clicked()
{
    Cnow = 1;
    changeLabel();
}

void Widget::on_pushButton_4_clicked()
{
    Cnow = 2;
    changeLabel();
}

void Widget::on_pushButton_5_clicked()
{
    Cnow = 3;
    changeLabel();
}

void Widget::on_pushButton_6_clicked()
{
    Cnow = 4;
    changeLabel();
}

void Widget::on_pushButton_7_clicked()
{
    Cnow = 0;
    changeLabel();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (!gaming) return;
    for(int i=1; i<num; i++) if (sqrt(pow(event->pos().x()-X[i],2)+pow(event->pos().y()-Y[i],2))<=R) {
        if (col[i]) cnt[col[i]]--;
        col[i]=Cnow;
        if (col[i]) cnt[col[i]]++;
        changeLabel();
        repaint();
        return;
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_0 : Cnow=0;  break;
        case Qt::Key_Q : Cnow=0;  break;
        case Qt::Key_1 : Cnow=1;  break;
        case Qt::Key_2 : Cnow=2;  break;
        case Qt::Key_3 : Cnow=3;  break;
        case Qt::Key_4 : Cnow=4;  break;
    }
   changeLabel();
}

void Widget::dfs(int o)
{
    if (dfs_cnt[C] >= best) return;
    if (baseTime.msecsTo(QTime::currentTime()) > 20*1000) return;
    if (o == num-1) {
        best = dfs_cnt[C];
        qDebug() << best;
        baseTime = QTime::currentTime();
        for(int i=0; i<num; i++) col[i]=dfs_col[i];
        changeLabel();
        repaint();
        return;
    }
    int x = st[o], c = 15, tmp = 0;
    for(std::vector<int>::iterator iter = v[x].begin(); iter != v[x].end(); iter++)
        if (dfs_col[*iter])
            c&=(15-(1<<(dfs_col[*iter]-1)));
        else if (!st_ed[*iter])
            st.push_back(*iter), st_ed[*iter]=true, tmp++;

    for(int i=1; i<=4; i++) if (i!=C && (c&(1<<(i-1)))) {
        dfs_col[x] = i;
        dfs_cnt[i] ++;
        dfs(o+1);
        dfs_col[x] = 0;
        dfs_cnt[i] --;
    }
    if (c&(1<<(C-1))) {
        dfs_col[x] = C;
        dfs_cnt[C] ++;
        dfs(o+1);
        dfs_col[x] = 0;
        dfs_cnt[C] --;
    }
    while (tmp--) st_ed[st[st.size()-1]]=false, st.pop_back();
}

void Widget::on_pushButton_2_clicked()
{
    for(int i=0; i<num; i++) dfs_col[i]=0;
    dfs_col[0]=col[0];
    for(int i=1; i<=4; i++) dfs_cnt[i]=0;
    dfs_cnt[dfs_col[0]]++;
    st.clear();
    memset(st_ed, 0, sizeof(st_ed));
    for(std::vector<int>::iterator iter = v[0].begin(); iter != v[0].end(); iter++) st.push_back(*iter), st_ed[*iter]=true;
    best = num;
    baseTime = QTime::currentTime();
    dfs(0);
    changeLabel();
    repaint();
}

void Widget::on_pushButton_8_clicked()
{
    for(int i=1; i<num; i++) col[i]=0;
    for(int i=1; i<=4; i++) cnt[i]=0;
    cnt[col[0]]++;
    changeLabel();
    repaint();
}
