# Milestones 

Milestones describe meaningful project outcomes and observable progress for Greenboy, a Game Boy emulator.

A milestone should answer the question:

> What meaningful thing can the emulator do now?

Milestones are intentionally:
- outcome-oriented
- implementation-agnostic
- observable
- iterative

Good milestone examples:

> The emulator can open a ROM.

> The emulator can show a screen.

> The emulator can react to input.

Bad milestone examples:

> Implement opcode decoder.

> Refactor timing model.

> Add scanline state machine.

Those are tasks or architectural concerns, not milestones.

# Milestone Structure

Milestones are organized by status:

```
/active
/planned
/completed
/vision
```

## Active

Milestones currently being worked on.

Usually there should only be one active milestone at a time.

## Planned

Near-term milestones that are reasonably understood and expected to be worked on next.

These should typically represent:
- the next 3–5 milestones
- clear goals
- rough acceptance criteria

## Completed

Finished milestones kept as historical records of project progress.

Completed milestones should not be heavily rewritten later.

## Vision

Long-term ideas and directional goals.

Vision documents are intentionally lightweight and may change significantly over time.

# Milestone Lifecycle

A milestone typically moves through the following stages:

```plantuml
hide empty description
[*] -> Vision
Vision -> Planned
Planned -> Active
Active -> Completed
Completed -> [*]
```

Not every idea becomes a milestone.

Not every milestone reaches completion unchanged.

Milestones may:
- split
- merge
- reorder
- become obsolete

as understanding of the emulator evolves.

# Writing Milestones

Milestones should focus on:
- visible capability
- meaningful progress
- user-observable outcomes

They should avoid:
- implementation details
- architectural specifics
- low-level technical work

A useful test is:

> Would someone observing the emulator notice this capability?

If the answer is no, it is probably:
- a task
- an implementation detail
- or an architecture concern

rather than a milestone.

# Scope Guidelines

Milestones should remain:
- focused
- achievable
- motivating

If a milestone grows too large, split it into smaller milestones.

Example:

> Display graphics

may later become:

> Show screen

> Render background tiles

> Render sprites

> Support scrolling

# Relationship To Tasks

Milestones define outcomes.

Tasks define implementation work.

Example:

> Milestone:
> The emulator can show a screen.
> 
> Tasks:
> - Create framebuffer abstraction
> - Add display window
> - Present placeholder frame

Tasks support milestones, but milestones should not become collections of technical tasks.

# Relationship To Architecture

Architecture documents explain:
- why the emulator is designed a certain way
- technical tradeoffs
- evolving decisions

Milestones describe:

* what the emulator can do

These are intentionally separate concerns.

Current Progress

See:
- [/active](active)
- [/planned](planned)
- [/completed](completed)
- [/vision](vision)

for the current state of the project.