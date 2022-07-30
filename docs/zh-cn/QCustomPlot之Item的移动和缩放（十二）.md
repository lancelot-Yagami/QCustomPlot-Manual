首先看一下效果图
![移动和缩放Item](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1hOGQyMjdkYjI3NWE0MjE5LmdpZg)

#### 移动
Item移动的主要思想是改变Item下所有QCPItemPosition的位置来达到移动的目的

```C++
void QCPSizeHandleManager::onItemMove(QCPAbstractItem *item, const QPointF &delta)
{
    if (!item || !mItems.contains(item))
        return;

    auto itemData = mItems.value(item);
    if (!itemData->movable)
        return;

    for (auto position : item->positions())
        position->setPixelPosition(position->pixelPosition() + delta);
}
```

#### 缩放
与移动稍微不同的是，缩放只是移动一个QCPItemPosition的位置就可以了，不过为了让缩放点可视化，我们在缩放点位置新增了一个QCPSizeHandle，QCPSizeHandle是一个自定义的Item，如果你还不会自定义Item，请看上篇[QCustomPlot之鼠标悬浮显示值（十一）](https://blog.csdn.net/qq10097355/article/details/105048146)

```C++
void QCPSizeHandleManager::onItemResize(QCPSizeHandle *sizeHandle, const QPointF &delta)
{
    if (!sizeHandle)
        return;

    auto itemData = mItems.value(sizeHandle->attachItem());
    if (!itemData || !itemData->resizable)
        return;

    if (auto parentPosition = static_cast<QCPItemPosition*>(sizeHandle->position->parentAnchor()))
    {
        parentPosition->setPixelPosition(parentPosition->pixelPosition() + delta);
    }
}
```

具体请查看[源码](https://github.com/lancelot-Yagami/QCustomPlot-Manual/tree/main/src/examples/012_stretch_item_demo)