class CanvasManager {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = this.canvas.getContext('2d');
    }

    drawWorld(worldData) {
        let pixelLength = Math.sqrt(worldData.Map.length);

        for (let i = 0; i < worldData.Map.length; i++) {
            let height = worldData.Map[i];
            console.log("This works");

            // Set color based on height
            if (height < 0) {
                this.ctx.fillStyle = '#0000ff';
            }
            else if (height < 250) {
                this.ctx.fillStyle = '#00624b';
            }
            else if (height < 500) {
                this.ctx.fillStyle = '#1a8235';
            }
            else if (height < 750) {
                this.ctx.fillStyle = '#4c9d41';
            }
            else if (height < 1000) {
                this.ctx.fillStyle = '#a7bf67';
            }
            else if (height < 1250) {
                this.ctx.fillStyle = '#dbcf7b';
            }
            else if (height < 1500) {
                this.ctx.fillStyle = '#c8974a';
            }
            else if (height < 1750) {
                this.ctx.fillStyle = '#a85917';
            }
            else if (height < 2000) {
                this.ctx.fillStyle = '#993b07';
            }
            else if (height < 2500) {
                this.ctx.fillStyle = '#86251e';
            }
            else if (height < 3000) {
                this.ctx.fillStyle = '#793c3b';
            }
            else if (height < 3500) {
                this.ctx.fillStyle = '#725859';
            }
            else if (height < 4000) {
                this.ctx.fillStyle = '#b0b0b0';
            }
            else if (height < 5500) {
                this.ctx.fillStyle = '#c8c8c8';
            }
            else if (height < 7000) {
                this.ctx.fillStyle = '#ececec';
            }
            else {
                this.ctx.fillStyle = '#ffffff';
            }

            // Calculate x, y coordinates
            let x = i % pixelLength;
            let y = Math.floor(i / pixelLength);
            this.ctx.fillRect(x, y, 1, 1);
        }
    }
}

const canvas = document.getElementById('canvas');
const canvasManager = new CanvasManager(canvas);

fetch('world.json')
    .then(response => response.json())
    .then(data => {
        canvasManager.drawWorld(data);
    })
    .catch(error => {
        console.error('Error loading world data:', error);
    });