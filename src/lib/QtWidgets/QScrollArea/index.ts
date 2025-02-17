import addon from "../../utils/addon";
import { NodeWidget } from "../QWidget";
import { BaseWidgetEvents } from "../../core/EventWidget";
import { NativeElement } from "../../core/Component";
import { QAbstractScrollArea } from "../QAbstractScrollArea";

export const QScrollAreaEvents = Object.freeze({
  ...BaseWidgetEvents
});
export class QScrollArea extends QAbstractScrollArea {
  native: NativeElement;
  contentWidget?: NodeWidget | null;
  constructor(parent?: NodeWidget) {
    let native;
    if (parent) {
      native = new addon.QScrollArea(parent.native);
    } else {
      native = new addon.QScrollArea();
    }
    super(native);
    this.native = native;
    this.parent = parent;
    // bind member functions
    this.setWidget.bind(this);
    this.takeWidget.bind(this);
  }
  setWidget(widget: NodeWidget) {
    // react:✓, //TODO:getter
    this.contentWidget = widget;
    this.native.setWidget(widget.native);
  }
  takeWidget(): NodeWidget | null {
    // react:✓
    const contentWidget = this.contentWidget;
    this.contentWidget = null;
    if (contentWidget) {
      this.native.takeWidget();
      return contentWidget;
    }
    return null;
  }
}
