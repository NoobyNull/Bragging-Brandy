📜 The Constitution of the CNC Catalogue & 3D Visualization Environment

Preamble

We, the designers and stewards of this system, establish this constitution to govern the creation of a Windows‑native, Qt‑based, DirectX‑accelerated CNC Catalogue and Visualization Environment. Its primary mission is the import, optimization, repair, and visualization of 3D models, while cataloguing them with related CNC files into coherent, metadata‑rich projects.

This environment shall be local‑first, modular, widget‑driven, transparent, and performance‑guaranteed. It shall serve hobbyists, small workshops, and educators with clarity, speed, and reliability. It shall never stall, never hide its workings, and never betray the sovereignty of its users’ data.

Article I – Pages & Widgets

Pages as Canvases

A Page is a workspace surface.

Users may create, rename, duplicate, and delete Pages.

Pages persist across sessions and can be exported/imported.

Widgets as Universal Units

Everything is a widget.

Examples: 3D Viewer, Metadata Editor, Material Library, Mesh Repair Report, File Browser, Search Panel, Background Scene Selector, Thumbnail Grid, Log Viewer.

Widgets can be added, removed, docked, nested, floated, resized, minimized, maximized.

Docking & Layout

Widgets snap into zones (left, right, top, bottom, center).

Widgets can be tabbed together.

Widgets can pop out into floating windows, movable across monitors.

Layouts are saved per Page and globally.

Article II – Visualization

3D Model Viewer Widget

DirectX 12 renderer embedded in Qt.

Supports pan, rotate, zoom, reset view, and 6‑axis space mouse input.

Sub‑3 second load time for models ≤200 MB.

Background Scenes

Options: solid color, gradient, grid plane, HDRI environment, custom image.

HDRI maps provide image‑based lighting (IBL).

Background adapts to Light/Dark/System theme.

User controls: HDRI intensity, rotation, ground plane toggle.

Material Rendering

PBR‑lite shaders for wood, acrylic, aluminum, etc.

Nested library: Species → Type.

Materials applied in real time.

Article III – Mesh Repair & Optimization

Repair Pipeline

Parse → Validate → Repair → Cache → Render.

Operations:

Duplicate vertex weld (epsilon threshold).

Non‑manifold edge cleanup.

Self‑intersection detection.

Normal/tangent recompute.

Degenerate triangle removal.

Performance

Repair budget: ≤1.5 seconds for ≤200 MB models.

Cache repaired meshes for instant reload.

Compute

GPU offload: DirectX 12 compute shaders.

Vendor paths: CUDA (NVIDIA), Level Zero (Intel).

CPU SIMD fallback (AVX2/AVX‑512).

Repair Report Widget

Displays counts of fixes, thresholds, unresolved issues.

Exportable as JSON/Markdown.

Article IV – Catalogue & Metadata

Catalogue Widget

Organizes projects into hierarchical folders.

Each project contains models, G‑code, cutsheets, previews, notes.

Metadata Editor Widget

Editable fields: project title, material, tooling, spindle/feed, cut depth, estimated time, tags, notes.

Batch editing and tagging supported.

Search Widget

Full‑text search across metadata and filenames.

Filters by tags, material, tooling, file type.

Performance: ≤500 ms for 50k entries.

Storage

SQLite database for metadata.

Binary cache for repaired meshes and thumbnails.

JSON for settings and layouts.

Article V – Thumbnails

Thumbnail Widget

Grid or list view of models/projects.

Auto‑generated on import.

Generation

Resolution: 256×256 px (configurable).

Viewpoint: standardized isometric (user override possible).

Background: matches active scene.

Performance: ≤500 ms per thumbnail.

Caching

Stored with checksum key.

Regenerated if background/material changes.

Article VI – Logging

Universal Logging

Every widget, every page, every process logs events.

Logs are structured, timestamped, and local‑first.

Log Structure

Format: JSONL or SQLite log table.

Fields: timestamp, page ID, widget ID, event type, details, performance metrics.

Log Viewer Widget

Dockable/floating widget to browse logs.

Filters by event type, widget, page, severity.

Exportable to JSON/CSV/text.

Performance

Asynchronous logging; never blocks UI.

Configurable retention and verbosity.

Article VII – Settings

Universal Settings Principle

Every option must be surfaced in Settings.

No hidden toggles.

Control Types

Checkboxes for binary toggles.

Radio buttons for exclusive choices.

Dropdowns for multiple options.

Sliders/numeric inputs for ranges.

File/color pickers for user assets.

Categories

General, Visualization, Mesh Repair, Catalogue, Thumbnails, Docking & Pages, GPU & Performance, Logging, CLI Integration.

UX

Windows 11 Fluent styling.

Tooltips for every option.

Reset‑to‑defaults button.

Article VIII – CLI

Switches:

--reset-database

--reset-settings

--import <path>

--export <path>

--gpu <auto|cpu|dgpu|adapterId>

--repair <auto|assist|off>

--theme <light|dark|system>

Article IX – Loading & Responsiveness

Loading Widgets

Any process >500 ms must show a loading widget.

Progress bar (deterministic) or spinner (indeterminate).

Always paired with status text.

Responsiveness

UI must never freeze.

All long tasks are asynchronous.

Crashes and silent stalls are unacceptable.

Article X – Performance Guarantees

Model load: ≤3 seconds for ≤200 MB.

Mesh repair: ≤1.5 seconds.

Thumbnail generation: ≤500 ms.

Background load: ≤500 ms.

Search: ≤500 ms for 50k entries.

Import indexing: ≤2 seconds per 100 files.

Article XI – Extensibility

Plugin system via Qt plugin architecture.

APIs for new widgets, file types, metadata fields, render passes.

Stable interfaces for catalogue access and GPU tasks.

Article XII – Boundaries

The application shall not:

Control CNC machines.

Generate or edit toolpaths.

Provide version control or revision history.

Depend on cloud services.

Optimize feeds/speeds automatically.

Article XIII – Governing Principles

Visualization‑first: Models must always be viewable, optimized, repaired.

Local‑first sovereignty: Data remains under user control.

Transparency: Metadata, logs, repair reports, thumbnails are visible.

Performance: Sub‑3 second loads, responsive search, progress indicators.

Modularity: Pages + Widgets as universal building blocks.

Accessibility: Windows 11‑native UI, intuitive controls, low learning curve.

Reliability: No freezes, no silent failures, no hidden states.