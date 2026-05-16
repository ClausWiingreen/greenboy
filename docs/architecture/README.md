# Architecture
Architecture documents describe important technical decisions, tradeoffs, and design direction for Greenboy.

They should answer the question:

> Why is Greenboy built this way?

They are not meant to document every implementation detail.

# Purpose

Architecture documents exist to preserve:
- important decisions
- reasoning
- alternatives considered
- tradeoffs
- consequences
- lessons learned

They are especially useful when a decision affects multiple parts of the emulator or may be difficult to reverse later.

# When To Write One

Write or update an architecture document when deciding something like:
- language and tooling
- project layout
- core/frontend boundary
- emulator loop
- timing model
- logging strategy
- testing strategy
- ROM asset policy

Do not write one for:
- small refactors
- local helper functions
- minor naming choices
- routine implementation details

# Document Style

Architecture documents should be short and practical.

A useful structure is:

```markdown
# Topic

## Current Approach

What the project currently does.

## Why

Why this approach was chosen.

## Alternatives Considered

Other reasonable options and why they were not chosen.

## Consequences

Positive and negative effects of the decision.

## Previous Approaches

Older approaches, if this document replaced or revised earlier thinking.
```

Use only the sections that are useful.

# Relationship To Milestones

Milestones describe:

> What the emulator can do.

Architecture describes:

> Why the emulator is designed this way.

Example:

Milestone:
>The emulator can run a ROM session.

Architecture:
>The emulator core is separated from CLI and desktop frontends.

# Relationship To Tasks

Tasks describe concrete work.

Architecture documents explain the reasoning behind that work.

Example:

Architecture:
> The CLI frontend is the first frontend because it supports testing and diagnostics.

Tasks:
- > Add CLI entrypoint
- > Parse ROM path
- > Print cartridge information

# Current Architecture Documents

Currently, there are no achitecture documents.

Add new documents only when the decision is important enough to preserve.