class CanvasManager {
    constructor(canvas) {
        this.canvas = canvas;
        this.ctx = this.canvas.getContext('2d');
    }

    drawWorld(worldData, seaLevel = 0) {
        const pixelLength = Math.sqrt(worldData.Map.length);
        const imageData = this.ctx.createImageData(pixelLength, pixelLength);
        const data = imageData.data;

        // Precompute color mapping for speed
        const colorMap = [
            [0, 0, 255],      // sea
            [0, 98, 75],      // < 7
            [26, 130, 53],    // < 14
            [76, 157, 65],    // < 21
            [167, 191, 103],  // < 28
            [219, 207, 123],  // < 35
            [200, 151, 74],   // < 42
            [168, 89, 23],    // < 49
            [153, 59, 7],     // < 56
            [134, 37, 30],    // < 70
            [121, 60, 59],    // < 84
            [114, 88, 89],    // < 98
            [176, 176, 176],  // < 112
            [200, 200, 200],  // < 155
            [236, 236, 236],  // < 198
            [255, 255, 255],  // else
        ];

        function getColor(height) {
            if (height <= seaLevel) return colorMap[0];
            else if (height < 7) return colorMap[1];
            else if (height < 14) return colorMap[2];
            else if (height < 21) return colorMap[3];
            else if (height < 28) return colorMap[4];
            else if (height < 35) return colorMap[5];
            else if (height < 42) return colorMap[6];
            else if (height < 49) return colorMap[7];
            else if (height < 56) return colorMap[8];
            else if (height < 70) return colorMap[9];
            else if (height < 84) return colorMap[10];
            else if (height < 98) return colorMap[11];
            else if (height < 112) return colorMap[12];
            else if (height < 155) return colorMap[13];
            else if (height < 198) return colorMap[14];
            else return colorMap[15];
        }

        for (let i = 0; i < worldData.Map.length; i++) {
            const color = getColor(worldData.Map[i]);
            const idx = i * 4;
            data[idx] = color[0];     // R
            data[idx + 1] = color[1]; // G
            data[idx + 2] = color[2]; // B
            data[idx + 3] = 255;      // A
        }

        this.ctx.putImageData(imageData, 0, 0);
    }
}

const canvas = document.getElementById('canvas');
const canvasManager = new CanvasManager(canvas);

const seaLevelSlider = document.getElementById('seaLevel');
const seaLevelValue = document.getElementById('seaLevelValue');

document.addEventListener('DOMContentLoaded', function() {
    if (typeof worldData !== 'undefined') {
        canvasManager.drawWorld(worldData, parseInt(seaLevelSlider.value, 10));
    } else {
        console.error('World data not loaded. Make sure world.js is properly generated.');
    }
});

seaLevelSlider.addEventListener('input', function() {
    seaLevelValue.textContent = seaLevelSlider.value;
    if (typeof worldData !== 'undefined') {
        canvasManager.drawWorld(worldData, parseInt(seaLevelSlider.value, 10));
    }
});