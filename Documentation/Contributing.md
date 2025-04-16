# Contributing to Renderwerk

Thank you for your interest in contributing to Renderwerk! This document provides guidelines for contributing to our C++ game engine project.

## Code Style

Renderwerk follows a code style similar to Unreal Engine. Here are the key guidelines:

- Use CamelCase for class names (e.g., `FRenderComponent`)
- Use PascalCase for function names (e.g., `ProcessMaterial()`)
- Prefix class names with appropriate letters:
  - `F` for regular classes (e.g., `FMeshRenderer`)
  - `T` for templates (e.g., `TArray`)
  - `E` for enums/enum classes (e.g., `EWindowMode`)
  - `I` for interfaces (e.g., `IWindowBase`)
- Use descriptive variable names
- Boolean variables should:
  - Start with a lowercase 'b' (e.g., `bIsVisible`, `bHasCollision`)
  - Be named as questions that can be answered Yes/No
  - Use positive phrasing (e.g., `bIsInitialized` instead of `bIsNotInitialized`)
- Braces on new lines
- Header files use `.hpp` extension, implementation files use `.cpp`

> For more information take a look at [Unreal Engines Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine)

Example:

```cpp
class FRenderSystem : public IEngineSystem
{
public:
  void Initialize() override;
  void Shutdown() override;

private:
	int32 ScreenWidth;
	int32 ScreenHeight;
	bool8 bShouldWaitForWindow;
};
```

## Commit Guidelines

We use Conventional Commits for our commit messages. Each commit message should be structured as follows:

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

Types include:

- `feat:` New features
- `fix:` Bug fixes
- `docs:` Documentation changes
- `style:` Code style changes (formatting, etc.)
- `refactor:` Code changes that neither fix bugs nor add features
- `perf:` Performance improvements
- `test:` Adding or modifying tests
- `chore:` Changes to the build process or auxiliary tools

Examples:

```
feat(renderer): add support for PBR materials
fix(physics): resolve collision detection issue with static meshes
test(core): add unit tests for entity component system
```

## Pull Request Process

1. Create a pull request from your feature branch to the develop branch
2. Ensure your code follows our style guidelines
3. Make sure all tests pass
4. Update documentation if necessary
5. Wait for the CI workflow to complete successfully
6. Address any feedback from code reviews

Our GitHub workflow automatically builds the project and runs tests when a PR is created.

## Testing

Always write tests for new functionality where it makes sense. We value:

- Unit tests for individual components
- Integration tests for systems that work together
- Performance tests for critical rendering paths

Use our testing framework and follow these guidelines:

- Test files should be named `*Test.cpp`
- Each test should be focused and test one specific behavior

Example:

```cpp
TEST(MeshRendererTests, LoadsTriangleMeshCorrectly)
{
    FMeshRenderer Renderer;
    bool Result = Renderer.LoadMesh("TestData/triangle.obj");

    EXPECT_TRUE(Result);
    EXPECT_EQ(Renderer.GetVertexCount(), 3);
    EXPECT_EQ(Renderer.GetFaceCount(), 1);
}
```

## Questions?

If you have questions about contributing, please open an issue or reach out to the maintainers.

Thank you for helping make Renderwerk better!
