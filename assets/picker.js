// https://github.com/umdjs/umd
(function (root, factory) {
    if (typeof define === "function" && define.amd) {
        // AMD. Register as an anonymous module.
        define(factory);
    } else if (typeof exports === "object") {
        // Node. Does not work with strict CommonJS
        module.exports = factory();
    } else {
        // Browser globals with support for web workers (root is window)
        root.picker = factory();
    }
}(this, function() {
    "use strict";

    var picker = function(canvasId) {
        this.throttle = true;
        this.canvas = document.getElementById(canvasId);
        this.context = this.canvas.getContext("2d");
        this.canvas.setAttribute("style", "cursor:crosshair");

        // Events
        this.canvas.addEventListener("mousedown", this.onMouseDown.bind(this));
        this.canvas.addEventListener("mouseup", this.onMouseUp.bind(this));
        this.canvas.addEventListener("touchstart", this.onTouchStart.bind(this));
        this.canvas.addEventListener("touchend", this.onTouchEnd.bind(this));

        this.boundMouseMove = this.onMouseMove.bind(this);
        this.boundTouchMove = this.onTouchMove.bind(this);

        this.draw(400, 400);
    }

    picker.prototype.draw = function(width, height) {
        this.canvas.width = width;
        this.canvas.height = height;

        // Draw palette
        var colorGradient = this.context.createLinearGradient(0, 0, this.canvas.width, 0);
        colorGradient.addColorStop(0, "rgb(255,0,0)");
        colorGradient.addColorStop(0.16, "rgb(255,0,255)");
        colorGradient.addColorStop(0.32, "rgb(0,0,255)");
        colorGradient.addColorStop(0.48, "rgb(0,255,255)");
        colorGradient.addColorStop(0.64, "rgb(0,255,0)");
        colorGradient.addColorStop(0.80, "rgb(255,255,0)");
        colorGradient.addColorStop(1, "rgb(255,0,0)");
        this.context.fillStyle = colorGradient;
        this.context.fillRect(0, 0, this.canvas.width, this.canvas.height);

        var bwGradient = this.context.createLinearGradient(0, 0, 0, this.canvas.height);
        bwGradient.addColorStop(0, "rgba(255,255,255,1)");
        bwGradient.addColorStop(0.5, "rgba(255,255,255,0)");
        bwGradient.addColorStop(0.5, "rgba(0,0,0,0)");
        bwGradient.addColorStop(1, "rgba(0,0,0,1)");
        this.context.fillStyle = bwGradient;
        this.context.fillRect(0, 0, this.canvas.width, this.canvas.height);
    }

    picker.prototype.onMouseDown = function(e) {
        this.onMouseMove(e);
        this.canvas.addEventListener("mousemove", this.boundMouseMove);
    }

    picker.prototype.onMouseUp = function(e) {
        this.canvas.removeEventListener("mousemove", this.boundMouseMove);
    }

    picker.prototype.onMouseMove = function(e) {
        e.preventDefault();

        if (this.throttle) {
            this.throttle = false;
            this.setColor(e.pageX - this.canvas.offsetLeft, e.pageY - this.canvas.offsetTop);

            setTimeout(function() {
                this.throttle = true;
            }.bind(this), 100);
        }
    }

    picker.prototype.onTouchStart = function(e) {
        this.onTouchMove(e);
        this.canvas.addEventListener("touchmove", this.boundTouchMove);
    }

    picker.prototype.onTouchEnd = function(e) {
        this.canvas.removeEventListener("touchmove", this.boundTouchMove);
    }

    picker.prototype.onTouchMove = function(e) {
        var touch = e.touches[0];
        this.setColor(touch.clientX, touch.clientY);
    }

    picker.prototype.setColor = function(x, y) {        
        var data = this.context.getImageData(x, y, 1, 1).data;
        var event = new CustomEvent("colorselected", {
            detail: {
                r: data[0], 
                g: data[1], 
                b: data[2]
            }
        });

        this.canvas.dispatchEvent(event);
    };

    return picker;
}));
