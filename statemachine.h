#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/// This library helps developers write finite state machines in C.
///
/// States are functions which return other states. State machine instances are
/// structures which keep track of the current state, a pointer to a context
/// variable, and some extra information the helper macros rely on.
///
/// Developers (should) use the context variable to pass information to the
/// state machine instance and read information from it; this makes it possible
/// to use the same states in several state machine instances at once.
///
/// Conceptually, state bodies comprise:
///
///   1. Entry logic;
///   2. Body logic; and
///   3. Exit logic.
///
/// Read the comment for `StateMachineRun()` to understand when each type of
/// logic runs and look at the documentation comment preceding the macro
/// definitions to see what a sample state looks like.

#include <stdbool.h>

#include "statemachine/internal/checked_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// Modeling states as functions which return other states requires a level
    /// of indirection to avoid recursive type declarations. Making states take
    /// state machine instances as arguments, while simultaneously storing
    /// states in state machines, requires a forward declaration.
    struct StateMachine_t;
    typedef struct SmState_t
    {
        struct SmState_t (*m_state)(struct StateMachine_t *a_sm);
    } SmState_t;

    /// The function type inside the `SmState_t` instance is a bit messy, so we
    /// introduce a `typedef` to tidy things up. Here a "raw" state is a
    /// function pointer outside of its `SmState_t` struct wrapper.
    typedef struct SmState_t (*SmRawState_t)(struct StateMachine_t *a_sm);

    /// All the information needed to drive state machine instances lives in a
    /// variable of type `StateMachine_t`.
    typedef struct StateMachine_t
    {
        // NOTE: Clients should never interact with these fields directly.

        SmRawState_t m_prev_state;  ///< State transition detection.
        SmRawState_t m_next_state;  ///< Inter-macro communication.
        SmState_t m_curr_state;     ///< Progress tracking.
        void *m_context;  ///< Facilitates communication between the inside and
                          /// outside of the state machine.
    } StateMachine_t;

    /// Prepares a state machine instance, `a_sm`, to start running from the
    /// `a_start` state, supported by the information in `a_context`. This must
    /// be called on `a_sm` before passing it to other state machine functions.
    void StateMachineInit(StateMachine_t *a_sm, SmRawState_t a_start,
                          void *a_context);

    /// Does work in the current state and returns true if the state machine can
    /// do more work. More specifically, if the current state is A, then:
    ///
    ///   1. Run A's entry logic if it was not the previous state.
    ///   2. Run A's body logic, which must either yield or signal a transition.
    ///     a) Yields leave A as the current state.
    ///     b) Transitions run A's exit logic and makes another state current.
    bool StateMachineRun(StateMachine_t *a_sm);

    /// Clients should not refer to this function directly. It is used as a
    /// sentinel value within state machine logic.
    SmState_t SmYieldSentinel(StateMachine_t *a_sm);

    /// The following macros use patterns which are best explained by:
    ///
    ///   https://www.chiark.greenend.org.uk/~sgtatham/mp/
    ///
    /// They are intended to let developers write state code which looks like
    /// the following:
    ///
    ///    SmState_t SomeState(StateMachine_t *a_sm) {
    ///      SM_ENTRY(a_sm) {
    ///        // Entry logic goes here. Can leave the block with 'break'.
    ///      }
    ///
    ///      /// Body logic goes here. All branches MUST result in a call to
    ///      /// either SM_YIELD() or SM_TRANSITION(a_sm, <some_state>).
    ///
    ///      SM_EXIT(a_sm) {
    ///        // Exit logic goes here. Can leave the block with 'break'.
    ///      }
    ///    }
    ///
    /// The macro implementations below may look scary, but they are relatively
    /// easy to understand after reading the reference above and they expand to
    /// efficient code.

#define SM_ENTRY(a_sm)                                              \
    if (1)                                                          \
    {                                                               \
        if (a_sm->m_curr_state.m_state != a_sm->m_prev_state)       \
        {                                                           \
            goto sm_entry_code;                                     \
        }                                                           \
    }                                                               \
    else                                                            \
    sm_entry_code:                                                  \
        /* Eat 'break' in user code to keep symmetry with EXIT().*/ \
        /* Use '$' in the identifier to avoid collisions.*/         \
        for (int $i = 0; $i < 1; ++$i)

#define SM_YIELD()                  \
    return (SmState_t)              \
    {                               \
        .m_state = SmYieldSentinel, \
    }

#define SM_YIELD_IF(cond) \
    if (cond)             \
    {                     \
        SM_YIELD();       \
    }

#define SM_TRANSITION(a_sm, a_raw_state)  \
    do                                    \
    {                                     \
        a_sm->m_next_state = a_raw_state; \
        goto sm_exit_target;              \
    } while (0)

#define SM_TRANSITION_IF(a_sm, cond, a_raw_state) \
    if (cond)                                     \
    {                                             \
        SM_TRANSITION(a_sm, a_raw_state);         \
    }

#define SM_EXIT(a_sm)                                                 \
    sm_exit_target:                                                   \
    if (1)                                                            \
    {                                                                 \
        goto sm_exit_body;                                            \
    }                                                                 \
    else                                                              \
        while (1)                                                     \
            if (1)                                                    \
            {                                                         \
                return (SmState_t){                                   \
                    .m_state = a_sm->m_next_state,                    \
                };                                                    \
            }                                                         \
            else                                                      \
            sm_exit_body:                                             \
                /* Eat 'break' in user code to avoid return bypass.*/ \
                /* Use '$' in the identifier to avoid collisions.*/   \
                for (int $i = 0; $i < 1; ++$i)

#ifdef __cplusplus
}
#endif

#endif /* STATEMACHINE_H */
