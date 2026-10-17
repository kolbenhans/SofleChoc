export function Name() { return "Sofle Choc RP (SignalRGB)"; }
export function Version() { return "1.0.0"; }
export function VendorId() { return 0xFC32; }
export function ProductId() { return 0x50F1; }
export function Publisher() { return "kolbenhans"; }
export function DeviceType() { return "keyboard"; }
export function Size() { return [58, 18]; }
export function DefaultPosition() { return [10, 100]; }
export function DefaultScale() { return 8.0; }
export function ImageUrl() { return "https://josefadamcik.github.io/SofleKeyboard/images/build_guide_choc/keyboard.jpeg"; }

/* global
shutdownMode:readonly
shutdownColor:readonly
LightingMode:readonly
forcedColor:readonly
*/
export function ControllableParameters() {
	return [
		{ "property": "shutdownMode", "group": "lighting", "label": "Shutdown Mode", "type": "combobox", "values": ["SignalRGB", "Hardware"], "default": "SignalRGB" },
		{ "property": "shutdownColor", "group": "lighting", "label": "Shutdown Color", "min": "0", "max": "360", "type": "color", "default": "#000000" },
		{ "property": "LightingMode", "group": "lighting", "label": "Lighting Mode", "type": "combobox", "values": ["Canvas", "Forced"], "default": "Canvas" },
		{ "property": "forcedColor", "group": "lighting", "label": "Forced Color", "min": "0", "max": "360", "type": "color", "default": "#009bde" },
	];
}

// Plugin Version: Built for Protocol V1.0.6 (SRGBmods QMK_Community_Module)
// LED positions pulled straight from this board's g_led_config
// (sofle_choc_rp.c), scaled down from firmware units (/4) to a small
// integer grid -- identity mapping (array index == LED index), no vKeys
// reindirection needed since g_led_config.point[] is already LED-index order.
// /4, not the originally-tried /14: that coarser divisor collapsed several
// real-but-close positions (e.g. both thumb clusters' inner key) onto the
// same grid cell, rendering as overlapping circles in SignalRGB's layout
// view instead of the actual staggered split shape. /4 is confirmed
// collision-free (checked all 58 positions for duplicates).
//
// Deliberately NOT doing the GMMK template's QMK-version/protocol-version/
// total-leds handshake reads here: those use device.read() in a blocking
// loop, which raced against Render()'s own device.write() calls on the same
// HID pipe once rendering started during Initialize() -- confirmed via the
// device console: the "response" bytes read back during the handshake were
// our own StreamLightingData StartLedIdx sequence (0,9,18,27,36,45,54)
// leaking back in, not real firmware replies. Only effectEnable() (a
// write-only, no-readback command) is load-bearing for lighting to work.
const LED_COUNT = 58;

const vKeyPositions = [
	[24, 2], [24, 5], [24, 9], [24, 12], [29, 16], [24, 16], [19, 16], [19, 12], [19, 8], [19, 5],
	[19, 1], [14, 1], [14, 5], [14, 8], [14, 12], [14, 16], [10, 16], [10, 12], [10, 8], [10, 5],
	[10, 1], [5, 2], [5, 5], [5, 9], [5, 12], [0, 12], [0, 9], [0, 5], [0, 2], [32, 2],
	[32, 5], [32, 9], [32, 12], [30, 16], [32, 16], [37, 16], [37, 12], [37, 8], [37, 5], [37, 1],
	[42, 1], [42, 5], [42, 8], [42, 12], [42, 16], [46, 16], [46, 12], [46, 8], [46, 5], [46, 1],
	[52, 2], [52, 5], [52, 9], [52, 12], [56, 12], [56, 9], [56, 5], [56, 2],
];

const vKeyNames = Array.from({ length: LED_COUNT }, (_, i) => `Led ${i}`);

export function LedNames() { return vKeyNames; }
export function LedPositions() { return vKeyPositions; }

export function Initialize() {
	device.log("=== Initialize() START ===");
	try {
		device.setControllableLeds(vKeyNames, vKeyPositions);
		effectEnable();
		device.log("=== Initialize() DONE ===");
	} catch (e) {
		device.log("!!! Initialize() EXCEPTION: " + e);
	}
}

export function Render() {
	try {
		sendColors();
	} catch (e) {
		device.log("!!! Render() EXCEPTION: " + e);
	}
}

export function Shutdown(SystemSuspending) {
	if (SystemSuspending) {
		sendColors("#000000"); // Go Dark on System Sleep/Shutdown
	} else if (shutdownMode === "SignalRGB") {
		sendColors(shutdownColor);
	} else {
		effectDisable();
	}
}

function effectEnable() {
	const w = device.write([0x00, 0x25], 32);
	device.log("effectEnable write() returned " + w);
	device.pause(30);
}

function effectDisable() {
	device.write([0x00, 0x26], 32);
	device.pause(30);
}

function grabColors(shutdown = false) {
	const rgbdata = [];
	for (let i = 0; i < LED_COUNT; i++) {
		const [x, y] = vKeyPositions[i];
		let color;
		if (shutdown) {
			color = hexToRgb(shutdownColor);
		} else if (LightingMode === "Forced") {
			color = hexToRgb(forcedColor);
		} else {
			color = device.color(x, y);
		}
		rgbdata.push(color[0], color[1], color[2]);
	}
	return rgbdata;
}

function sendColors(shutdown = false) {
	const rgbdata = grabColors(shutdown);
	const LedsPerPacket = 9;
	let BytesSent = 0;
	let BytesLeft = rgbdata.length;

	while (BytesLeft > 0) {
		const BytesToSend = Math.min(LedsPerPacket * 3, BytesLeft);
		StreamLightingData(Math.floor(BytesSent / 3), rgbdata.splice(0, BytesToSend));
		BytesLeft -= BytesToSend;
		BytesSent += BytesToSend;
	}
}

function StreamLightingData(StartLedIdx, RGBData) {
	const packet = [0x00, 0x24, StartLedIdx, Math.floor(RGBData.length / 3)];
	packet.push(...RGBData);
	device.write(packet, 33);
}

function hexToRgb(hex) {
	const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
	return [parseInt(result[1], 16), parseInt(result[2], 16), parseInt(result[3], 16)];
}

export function Validate(endpoint) {
	return endpoint.interface === 1;
}
