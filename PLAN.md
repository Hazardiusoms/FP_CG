# Weeks 8 to 10 delivery plan

The final project is a four-member team task. The exact defense date remains unconfirmed. All publication is performed personally by team members, never AI.

## Week 8 — working module foundations

**Almas:** review the current indexed mesh, road, orbit camera, FPS/help and shared input. Run CPU/GPU checks, explain matrices and road phase, and supply interfaces and actual evidence. This foundation is implemented locally. Coordinate main.cpp and CMake integration changes.

**Turar:** build the car in the shared application. Body is the parent; wheels and headlights have local transforms. Clamp steering to the eight-unit road, smooth body tilt and rotate wheels by distance traveled. Provide a normal for every lit vertex and a screenshot proving independent wheel motion while the hierarchy follows the body.

**Muslim:** create distinct left/right mountain formations and a procedural sky/sun. Keep the horizon tied to camera orientation, ignore sky translation, and restore depth state after the background pass. Use indexed geometry for mountains. Verify both camera modes and portrait resizing.

**Nursalim:** implement world-space per-fragment Phong/Blinn–Phong with at least two lights. Make one light movable or toggleable using keys that do not conflict with the camera. Test normals under nonuniform scale and specular movement. Agree material parameters with Turar before integration.

Each member supplies source, a screenshot, exact local checks, limitations, credits and an honest AI-use record. AGENTS.md contains detailed step-by-step instructions for each role.

## Week 9 — integration and evidence

Integrate one module at a time through Almas; rerun CPU/GPU checks after every integration. All four inspect the same runnable build. Verify five distinct objects, hierarchy, two lights, procedural surface and keyboard/mouse camera before optional effects. Add bloom and CRT only after R1–R8 are stable. Use three meaningful scene interactions in the defense.

Turar records hierarchy/steering evidence; Muslim records environment/sun evidence; Nursalim measures performance with effects on/off and investigates failures; Almas verifies a fresh checkout on a second machine and synchronizes README, report and control list. All members contribute to the comparison of 15 journal sources; verify Q1/Q2 ranking evidence rather than assuming it from a publisher's name.

## Week 10 — freeze before the deadline and defend

Calculate the deadline from the confirmed defense date: three days earlier, 23:59 Asia/Almaty. Freeze source before that cutoff, not on defense day. Update the progress report into the final report only after screenshots, measurements, contributions and references match the integrated build. Record a 3–6 minute 1080p demo with live FPS; finish 8–12 slides and rehearse a 7–10 minute talk with all four members.

A human verifies TA repository access, creates v1.0 on the submission date and submits required files/links in Moodle. Keep a local backup and confirm uploaded files open. During defense, every member must be able to explain their own work and shared rendering flow. Do not claim two stretch features unless both work in the submitted build.
