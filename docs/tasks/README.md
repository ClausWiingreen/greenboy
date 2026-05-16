# Tasks

Tasks describe concrete work needed to move Greenboy forward.

They should answer the question:

> What should be done next?

Tasks are intentionally smaller and more practical than milestones.

# Purpose

Tasks exist to:
- break milestones into actionable work
- keep scope visible
- prevent hidden work from accumulating
- capture discoveries during implementation
- make progress easier to resume later

A task should usually support a milestone or an architecture decision.

# Task Structure

Tasks may be organized by status:

```
/docs/tasks
    /open
    /active
    /done
```

Start simple. Add more folders only when needed.

# Task Lifecycle

A task normally moves through:

```plantuml
hide empty description
[*] -> Open
Open -> Active
Active -> Done
Done -> [*]
```

## Open

The task is known, but not currently being worked on.

A task can be open even if details are still rough.

## Active

The task is currently being worked on.

Usually there should only be one active task at a time.

## Done

The task has been completed and no longer needs active attention.

Completed tasks may be kept for history or removed after the related milestone is completed.

# Writing Tasks

A good task is:

- specific
- small
- actionable
- verifiable

Good examples:

> Add CLI option for ROM path.

> Print cartridge title.

> Return clear error for missing ROM file.

Bad examples:

> Implement emulator.

> Fix architecture.

> Make graphics work.

Those are too broad.

# Suggested Task Template

```markdown
# Task Title

## Status

Open

## Related Milestone

Link to milestone.

## Goal

What this task should accomplish.

## Done When

- Clear completion criterion
- Another completion criterion

## Notes

Useful context, discoveries, or constraints.
```

Keep task files short.

# Scope Rule

A task should not silently grow.

When implementation reveals new work create a new task rather than expanding the current one.

Example:

Current task:

> Print cartridge title.

Discovery:

> Some ROM titles contain padded bytes.

Create a new task:

> Normalize cartridge title output.

Only include it in the current task if it is required for the current task to be done.

# Relationship To Milestones

Milestones describe outcomes.

Tasks describe work.

Example:

Milestone:
> Open a Game Boy ROM and display basic cartridge information.

Tasks:
- > Create initial CLI entrypoint
- > Read ROM file from disk
- > Validate ROM size
- > Print cartridge title

A milestone is complete when its acceptance criteria are met, not when every possible related task is done.

> Relationship To Architecture

Architecture documents explain technical reasoning.

Tasks implement or support that reasoning.

Example:

Architecture:
> The emulator core should be separate from frontends.

Tasks:
- > Create core module
- > Create CLI frontend module
- > Ensure CLI depends on core

If a task reveals an important design decision, update or create an architecture document.

# Working On A Task

Before starting:
- read the related milestone
- read relevant architecture documents
- check what "done" means
- move the task to active
- create a feature branch

During work:
- keep changes focused
- add tests where useful
- record discoveries as notes or new tasks
- avoid unrelated refactoring

Before finishing:
- build succeeds
- tests pass
- done criteria are met
- related docs are updated if needed

# Branch Naming

Use a branch name based on the task:

`feature/001-add-cli-rom-path`

`feature/002-print-cartridge-title`

For fixes:

`fix/003-missing-rom-error`

# Task Completion

A task is done when:
- the goal has been achieved
- done criteria are met
- the change is tested or manually verified
- the branch has been merged
- follow-up work is captured separately

After completion, move the task to `/done`.

# Maintenance

Review tasks regularly.

During review:
- remove obsolete tasks
- split oversized tasks
- move completed tasks out of active work
- ensure tasks still support current milestones

The task list should remain useful, not become a graveyard.
