#include "mainview.ih"

// Triggered by pressing a key
void MainView::keyPressEvent(QKeyEvent *ev)
{
    if(ev->modifiers() & Qt::AltModifier)
        altModActive = true;
    if (ev->key() == Qt::Key_W)
        holdW = true;
    if (ev->key() == Qt::Key_A)
        holdA = true;
    if (ev->key() == Qt::Key_S)
        holdS = true;
    if (ev->key() == Qt::Key_D)
        holdD = true;
    if (ev->key() == Qt::Key_Q)
        holdQ = true;
    if (ev->key() == Qt::Key_E)
        holdE = true;
    if (ev->key() == Qt::Key_Space)
        holdSpace = true;
    if (ev->key() == Qt::Key_Shift)
        holdShift = true;
    if (ev->key() == Qt::Key_Control)
        holdCtrl = true;
    if (ev->key() == Qt::Key_Period)
        selectNextEntity(1);
    if (ev->key() == Qt::Key_Comma)
        selectNextEntity(-1);
}

// Triggered by releasing a key
void MainView::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->modifiers() ^ Qt::AltModifier)
        altModActive = false;
    if (ev->key() == Qt::Key_W)
        holdW = false;
    if (ev->key() == Qt::Key_A)
        holdA = false;
    if (ev->key() == Qt::Key_S)
        holdS = false;
    if (ev->key() == Qt::Key_D)
        holdD = false;
    if (ev->key() == Qt::Key_Q)
        holdQ = false;
    if (ev->key() == Qt::Key_E)
        holdE = false;
    if (ev->key() == Qt::Key_Space)
        holdSpace = false;
    if (ev->key() == Qt::Key_Shift)
        holdShift = false;
    if (ev->key() == Qt::Key_Control)
        holdCtrl = false;
}

// Triggered by clicking two subsequent times on any mouse button
// It also fires two mousePress and mouseRelease events!
void MainView::mouseDoubleClickEvent(QMouseEvent *ev)
{
//    update();
}

// Triggered when moving the mouse inside the window (only when the mouse is clicked!)
void MainView::mouseMoveEvent(QMouseEvent *ev)
{
//    update();
}

// Triggered when pressing any mouse button
void MainView::mousePressEvent(QMouseEvent *ev)
{
//    update();

    // Do not remove the line below, clicking must focus on this widget!
    this->setFocus();
}

// Triggered when releasing any mouse button
void MainView::mouseReleaseEvent(QMouseEvent *ev)
{
//    update();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainView::wheelEvent(QWheelEvent *ev)
{
    int scale    = getScale();
    int newScale = scale + (ev->delta() > 0 ? 2 : -2);
    newScale = newScale > 242.0f ? 242.0f : newScale < 50.0f ? 50.0f : newScale;
    setScale(newScale);

    emit scaleValueChanged();

    update();
}
