const std = @import("std");
const rl = @import("raylib");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const raylib_dep = b.dependency("raylib", .{
        .target = target,
        .optimize = optimize,
    });

    const raylib_artifact = raylib_dep.artifact("raylib");
    const exe = b.addExecutable(.{
        .name = "tic-tac-toe",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });

    exe.root_module.addIncludePath(raylib_dep.path("src"));

    exe.root_module.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .flags = &.{ "-std=c99", "-Wall", "-Wextra", "-pedantic" },
    });

    exe.root_module.linkLibrary(raylib_artifact);
    const run_step = b.step("run", "Run the game");

    if (target.query.os_tag == .emscripten) {
        const emsdk = rl.emsdk;

        const wasm = b.addLibrary(.{
            .name = "tic_tac_toe_web",
            .root_module = exe.root_module,
        });

        const install_dir: std.Build.InstallDir = .{
            .custom = "web",
        };

        const emcc_flags =
            emsdk.emccDefaultFlags(b.allocator, .{ .optimize = optimize });

        const emcc_settings =
            emsdk.emccDefaultSettings(b.allocator, .{ .optimize = optimize });

        const emcc_step =
            emsdk.emccStep(
                b,
                raylib_artifact,
                wasm,
                .{
                    .optimize = optimize,
                    .flags = emcc_flags,
                    .settings = emcc_settings,
                    .shell_file_path = emsdk.shell(raylib_dep),
                    .install_dir = install_dir,
                },
            );

        b.getInstallStep().dependOn(emcc_step);
    } else {
        b.installArtifact(exe);

        const run_cmd = b.addRunArtifact(exe);

        run_cmd.step.dependOn(b.getInstallStep());

        if (b.args) |args|
            run_cmd.addArgs(args);

        run_step.dependOn(&run_cmd.step);
    }
}
