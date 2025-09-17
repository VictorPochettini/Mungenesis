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
            [25, 47, 35],    // < -14
            [10, 61, 33],     // < -7
            [4, 84, 39],      // < 0
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

        const PochettiniColorMap = [
            [255, 255, 255], //No plate
            [255, 0, 0],     //Plate 1
            [0, 255, 0],     //Plate 2
            [0, 0, 255],     //Plate 3
            [255, 255, 0],   //Plate 4
            [0, 255, 255],   //Plate 5
            [255, 0, 255],   //Plate 6
            [192, 192, 192], //Plate 7
            [128, 0, 0],     //Plate 8
            [128, 128, 0],   //Plate 9
        ];

        function getColor(height) {
            if (height <= seaLevel) return colorMap[0];
            else if (height < -14) return colorMap[1];
            else if (height < -7) return colorMap[2];
            else if (height < 0) return colorMap[3];
            else if (height < 7) return colorMap[4];
            else if (height < 14) return colorMap[5];
            else if (height < 21) return colorMap[6];
            else if (height < 28) return colorMap[7];
            else if (height < 35) return colorMap[8];
            else if (height < 42) return colorMap[9];
            else if (height < 49) return colorMap[10];
            else if (height < 56) return colorMap[11];
            else if (height < 70) return colorMap[12];
            else if (height < 84) return colorMap[13];
            else if (height < 98) return colorMap[14];
            else if (height < 112) return colorMap[15];
            else if (height < 155) return colorMap[16];
            else if (height < 198) return colorMap[17];
            else return colorMap[18];
        }

        function getPochettiniColor(plateId) {
            return PochettiniColorMap[plateId];
        }
// I have to think how I'll actually get the plateId from the JSON (the main problem actually lies on knowing how I'll store it)
        for (let i = 0; i < worldData.Map.length; i++) {
            const color = worldData.algorithmChoice == 2 ? getPochettiniColor(worldData.plateId) : getColor(worldData.Map[i]);
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


seaLevelValue.addEventListener('input', function() {
    let val = parseInt(seaLevelValue.value, 10);
    if (isNaN(val)) val = seaLevelSlider.min;

    val = Math.max(parseInt(seaLevelSlider.min, 10), Math.min(parseInt(seaLevelSlider.max, 10), val));
    seaLevelSlider.value = val;
    if (typeof worldData !== 'undefined') {
        canvasManager.drawWorld(worldData, val);
    }
});


seaLevelSlider.addEventListener('input', function() {
    seaLevelValue.value = seaLevelSlider.value;
    if (typeof worldData !== 'undefined') {
        canvasManager.drawWorld(worldData, parseInt(seaLevelSlider.value, 10));
    }
});

canvas.addEventListener('click', function(event) {
    console.log('Canvas clicked at:', event.offsetX, event.offsetY);
})