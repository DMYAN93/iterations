# Iterations — Character Matrix Design

## Overview
The personality matrix is the core feature of Iterations. Every meaningful character in the simulation has one. It defines how they think, what they want, how they react to events, and how they relate to other characters and the player. The matrix is what makes characters feel like people rather than stat blocks.

All emergent narrative — betrayals, alliances, loyalty, resentment, sacrifice — flows from the interaction of matrices between characters and with the simulation's events.

---

## Layer 1 — Core Dimensions
A single set of bipolar spectrums that define both what a character values and how they express it. Values and behavior are not separated — a character's personality *is* their value system in action.

Dimensions are stable but not immutable. They shift very slowly in response to sustained pressure from the simulation — a lifetime of trauma can change a person, but a bad week shouldn't.

**Proposed dimensions (subject to revision):**
- Loyalty vs. Self-interest
- Empathy vs. Cruelty
- Courage vs. Caution
- Tradition vs. Ambition
- Honesty vs. Pragmatism
- Curiosity vs. Certainty

These are not good/evil alignments. Every position on every spectrum is a coherent worldview held by real people. A pragmatic character isn't a villain — they just solve problems differently to an honest one.

Open question: should dimensions be continuous floats, discrete tiers, or something else?

---

## Layer 2 — Internal State (Harmony / Stress axis)

A bipolar metric representing the character's internal psychological coherence.

**Harmony** — the character is acting in accordance with their core dimensions. Provides stat boosts and a buffer before stress can accumulate. Hard to lose once established.

**Stress** — the character is being forced to act against their core dimensions, or has witnessed events that violate their values. Accumulates faster than harmony is gained — psychologically realistic asymmetry. Above a threshold, stress produces behavioral consequences: erratic decisions, value drift, breakdown, acting out.

**Memory modulation** — base harmony/stress levels are permanently shifted by significant memories. A character who experienced a traumatic event related to a specific person, place, or type of situation carries a lower harmony baseline in similar future situations. Trauma leaves a mark on the baseline, not just a one-time response.

Example: a character whose family was killed in a siege has a permanently lower harmony baseline in situations involving sieges or the faction responsible. It takes less stress to push them into consequences in those contexts.

---

## Layer 3 — External State (Affinity / Dissonance axis)

A bipolar metric representing the character's relational state toward a specific other character, faction, or group. Directional — character A's dissonance toward character B is independent of B's dissonance toward A.

**Affinity** — interactions and observed behavior align with the character's core dimensions and memories. Provides a buffer before dissonance accumulates. High affinity enables deeper cooperation, loyalty, and trust behaviors.

**Dissonance** — the other party acts in ways that violate this character's core dimensions, or reminds them of negative memories. Accumulates when perceived actions conflict with values. Above a threshold, produces behavioral consequences: withdrawal, friction, defection, hostility.

**Memory modulation** — same principle as internal state. A character has a permanently lower affinity baseline toward people or groups associated with negative memories. The simulation remembers who did what, and so do the characters.

Example: a lord whose son was killed by the player's grandfather has a lower base affinity toward the player's dynasty before any interaction has even occurred. The player inherits the history.

---

## Interaction Between Layers

The three layers are not independent — they feed into each other:

- **Core dimensions determine what registers as stress or dissonance.** A cruel character doesn't accumulate stress from ordering executions. An empathetic one does. The matrix defines what counts as a violation.
- **High internal stress amplifies dissonance accumulation.** A character under stress is more sensitive to perceived slights from others. A desperate character finds more things offensive.
- **High dissonance toward the player can cause internal stress.** Being forced to work alongside someone you resent is itself a stressor. The two axes compound.
- **Affinity buffers dissonance, not indefinitely.** A character with high affinity toward you can absorb more dissonant actions before consequences emerge. But affinity has a ceiling, and dissonance accumulated past it degrades affinity permanently.

---

## Behavioral Consequences

When stress or dissonance cross thresholds, the simulation triggers behavioral shifts. These are not scripted events — they emerge from state crossing thresholds combined with context.

Possible consequences at high stress:
- Acting against own values (the CK3 stress break equivalent)
- Seeking relief through vice, religion, or withdrawal
- Value drift — sustained stress can slowly shift core dimensions over years
- Breakdown — temporary stat penalties, erratic behavior

Possible consequences at high dissonance toward player:
- Withdrawal from cooperation
- Passive sabotage
- Open defection
- Alliance with player's enemies
- Assassination attempt (if courage dimension supports it)

Positive states have consequences too:
- High harmony: better decisions, leadership bonuses, inspires others
- High affinity toward player: loyalty beyond what obligation requires, sacrifice behaviors, sharing information unprompted

---

## Memory System

Memory is selective and weighted by emotional significance, not recency or volume. The simulation does not log everything — it logs what a person would actually remember and carry.

**What gets stored:**
- Events involving death of close relations
- Personal betrayals or acts of loyalty
- Moments of humiliation or honor
- Interactions with the player that crossed a threshold (positive or negative)
- Events that violated or affirmed core dimensions strongly

**How memory affects the matrix:**
- Permanently shifts base harmony/stress levels in related contexts
- Permanently shifts base affinity/dissonance toward related characters or groups
- Can surface in dialogue — characters reference their history when it's relevant
- Informs what a character considers a significant event going forward

---

## Open Questions
- Are core dimensions continuous floats, discrete tiers, or something else?
- How many dimensions is enough vs. too many? Current proposal is six — is that the right granularity?
- Should dimensions be visible to the player, or inferred from observed behavior?
- How does the matrix interact with the LLM dialogue layer — does the LLM receive the full matrix as context, or a summarized description?
- At what point does a background character ("Peasant 12") get a full matrix vs. a simplified version?
- How are matrices initialized for procedurally generated characters — pure random, constrained by faction, inherited from parents?
- Should children inherit matrix values from parents with variation, making family lines feel consistent over generations?
