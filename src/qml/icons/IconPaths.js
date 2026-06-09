.pragma library
// Single-color stroke icon paths, ported 1:1 from the design's icons.jsx.
// Each entry is an array of SVG path "d" strings on a 24x24 viewBox.
var ICONS = {
    notebook: ["M5 4h11a2 2 0 0 1 2 2v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1z", "M9 4v16", "M5 9H3M5 13H3"],
    tag: ["M4 12.5V5a1 1 0 0 1 1-1h7.5L20 11.5 13.5 18z", "M8.5 8.5h.01"],
    search: ["M11 18a7 7 0 1 0 0-14 7 7 0 0 0 0 14z", "m20 20-3.2-3.2"],
    snippet: ["M9 7 5 11l4 4", "m15 7 4 4-4 4", "M13 5l-2 14"],
    history: ["M3 12a9 9 0 1 0 3-6.7L3 8", "M3 4v4h4", "M12 8v4l3 2"],
    settings: ["M12 15.5a3.5 3.5 0 1 0 0-7 3.5 3.5 0 0 0 0 7z", "M19.4 13.5a1.6 1.6 0 0 0 .3 1.8l.05.05a2 2 0 1 1-2.8 2.8l-.05-.05a1.6 1.6 0 0 0-2.7 1.1V21a2 2 0 0 1-4 0v-.1a1.6 1.6 0 0 0-2.7-1.1l-.05.05a2 2 0 1 1-2.8-2.8l.05-.05a1.6 1.6 0 0 0-1.1-2.7H3a2 2 0 0 1 0-4h.1a1.6 1.6 0 0 0 1.1-2.7l-.05-.05a2 2 0 1 1 2.8-2.8l.05.05a1.6 1.6 0 0 0 1.8.3 1.6 1.6 0 0 0 1-1.5V3a2 2 0 0 1 4 0v.1a1.6 1.6 0 0 0 1 1.5 1.6 1.6 0 0 0 1.8-.3l.05-.05a2 2 0 1 1 2.8 2.8l-.05.05a1.6 1.6 0 0 0-.3 1.8z"],
    folder: ["M3 7a2 2 0 0 1 2-2h3.5l2 2.2H19a2 2 0 0 1 2 2V18a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1z"],
    file: ["M6 3h8l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z", "M14 3v4h4"],
    md: ["M4 16V8h2l2 3 2-3h2v8", "M16 8v5", "M14 11l2 2 2-2"],
    chevR: ["m9 6 6 6-6 6"],
    chevD: ["m6 9 6 6 6-6"],
    plus: ["M12 5v14", "M5 12h14"],
    close: ["M6 6l12 12", "M18 6 6 18"],
    save: ["M5 4h11l3 3v12a1 1 0 0 1-1 1H5a1 1 0 0 1-1-1V5a1 1 0 0 1 1-1z", "M8 4v5h7", "M8 14h8"],
    eye: ["M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7-10-7-10-7z", "M12 15a3 3 0 1 0 0-6 3 3 0 0 0 0 6z"],
    bold: ["M7 5h6a3.5 3.5 0 0 1 0 7H7z", "M7 12h7a3.5 3.5 0 0 1 0 7H7z"],
    italic: ["M10 5h7M7 19h7M14 5l-4 14"],
    strike: ["M5 12h14", "M8 8a3.5 3 0 0 1 3.5-3h2A3 3 0 0 1 16 7", "M9 16a3 3 0 0 0 3 2h1a3.2 3 0 0 0 3-3"],
    listU: ["M9 6h11M9 12h11M9 18h11", "M4.5 6h.01M4.5 12h.01M4.5 18h.01"],
    listO: ["M10 6h10M10 12h10M10 18h10", "M4 5l1-.5V9", "M3.5 9h2"],
    check: ["M9 11l3 3L22 4", "M21 12v7a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V5a1 1 0 0 1 1-1h11"],
    code: ["M8 7 3 12l5 5", "m16 7 5 5-5 5"],
    quote: ["M7 7c-2 0-3 1.5-3 3.5S5.5 14 7 14c.5 2-1 3-2 3.5M17 7c-2 0-3 1.5-3 3.5S15.5 14 17 14c.5 2-1 3-2 3.5"],
    link: ["M10 14a4 4 0 0 0 5.7 0l3-3a4 4 0 0 0-5.7-5.7L11 7", "M14 10a4 4 0 0 0-5.7 0l-3 3a4 4 0 0 0 5.7 5.7L13 17"],
    image: ["M4 5h16a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V6a1 1 0 0 1 1-1z", "M8.5 11a1.5 1.5 0 1 0 0-3 1.5 1.5 0 0 0 0 3z", "m4 16 4.5-4 4 3.5L16 11l5 5"],
    table: ["M4 5h16a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V6a1 1 0 0 1 1-1z", "M3 10h18M3 15h18M9 5v14M15 5v14"],
    heading: ["M6 5v14M18 5v14M6 12h12"],
    sigma: ["M5 5h13l-6 7 6 7H5"],
    split: ["M4 5h16a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V6a1 1 0 0 1 1-1z", "M12 5v14"],
    more: ["M5 12h.01M12 12h.01M19 12h.01"],
    moreV: ["M12 5h.01M12 12h.01M12 19h.01"],
    sort: ["M7 4v16M7 20l-3-3M7 4l3 3", "M14 7h6M14 12h4M14 17h2"],
    pin: ["M9 4h6l-1 6 3 3v2h-5v5l-1 2-1-2v-5H4v-2l3-3z"],
    book: ["M5 4h11a2 2 0 0 1 2 2v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1z", "M9 4v16"],
    star: ["M12 4l2.3 4.8 5.2.7-3.8 3.6 1 5.1L12 16l-4.7 2.2 1-5.1L4.5 9.5l5.2-.7z"],
    clock: ["M12 21a9 9 0 1 0 0-18 9 9 0 0 0 0 18z", "M12 7v5l3 2"],
    moon: ["M20 14.5A8 8 0 1 1 9.5 4a6.5 6.5 0 0 0 10.5 10.5z"],
    sun: ["M12 17a5 5 0 1 0 0-10 5 5 0 0 0 0 10z", "M12 2v2M12 20v2M4 12H2M22 12h-2M5 5l1.5 1.5M17.5 17.5 19 19M19 5l-1.5 1.5M6.5 17.5 5 19"],
    cmd: ["M9 6a2 2 0 1 0-2 2h10a2 2 0 1 0-2-2v10a2 2 0 1 0 2-2H7a2 2 0 1 0 2 2z"],
    grid: ["M4 4h7v7H4zM13 4h7v7h-7zM4 13h7v7H4zM13 13h7v7h-7z"],
    arrowR: ["M5 12h14M13 6l6 6-6 6"],
    inbox: ["M4 13h4l2 3h4l2-3h4", "M4 13 6 5h12l2 8v5a1 1 0 0 1-1 1H5a1 1 0 0 1-1-1z"],
    edit: ["M4 20h4L19 9a2 2 0 0 0-3-3L5 17z", "M14 6l3 3"],
    sidebar: ["M4 5h16a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V6a1 1 0 0 1 1-1z", "M9 5v14"],
    attach: ["M19 11l-7.5 7.5a4 4 0 0 1-5.7-5.7L13 5a2.7 2.7 0 0 1 3.8 3.8l-7.2 7.2a1.4 1.4 0 0 1-2-2l6.6-6.6"],
    flash: ["M13 3 5 14h6l-1 7 8-11h-6z"],
    import: ["M12 3v12", "M8 11l4 4 4-4", "M5 19h14"],
    export: ["M12 15V3", "M8 7l4-4 4 4", "M5 19h14"],
    dot: ["M12 12h.01"],
    word: ["M4 7V5h16v2", "M9 5v14M7 19h4"]
};

function get(name) {
    return ICONS[name] || [];
}

function joined(name) {
    // Lucide ships each subpath as its own <path>, so a leading relative 'm dx dy' is
    // measured from the origin (== absolute dx,dy). When we concatenate subpaths into one
    // path string, that 'm' would instead be relative to the previous subpath's end and
    // displace the glyph. Fix per non-first subpath: make the leading move absolute (M),
    // and if implicit coordinate pairs follow, keep them relative via an explicit 'l'.
    return get(name).map(function (p, i) {
        if (i === 0 || p.charAt(0) !== "m") return p;
        var m = p.match(/^m\s*(-?[\d.]+)[ ,]+(-?[\d.]+)(.*)$/);
        if (!m) return p;
        var head = "M" + m[1] + " " + m[2];
        var rest = m[3];
        return /^\s*[-\d.]/.test(rest) ? head + " l" + rest.replace(/^\s+/, "") : head + rest;
    }).join(" ");
}
