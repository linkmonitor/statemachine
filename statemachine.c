#include "statemachine.h"

#include <string.h>

void StateMachineInit(StateMachine_t *a_sm, SmRawState_t a_start_state,
                      void *a_context)
{
    SM_ASSERT(a_sm != NULL);
    SM_ASSERT(a_start_state != NULL);
    memset(a_sm, 0, sizeof(StateMachine_t));
    a_sm->m_curr_state.m_state = a_start_state;
    a_sm->m_context            = a_context;
}

bool StateMachineRun(StateMachine_t *a_sm)
{
    SM_ASSERT(a_sm != NULL);
    SmRawState_t curr = a_sm->m_curr_state.m_state;
    SM_ASSERT(curr != NULL);
    a_sm->m_next_state = NULL;
    SmState_t next     = curr(a_sm);
    if (next.m_state == SmYieldSentinel)
    {
        // Stay in the same state if the client yielded.
        next.m_state = curr;
    }
    else
    {
        // Clients MAY NOT transition to the current state, they MUST yield.
        SM_ASSERT(next.m_state != curr);
        // This is NULL if the state body reached SM_EXIT() without first
        // calling SM_YIELD() or SM_TRANSITION(), which is disallowed.
        SM_ASSERT(next.m_state != NULL);
    }
    a_sm->m_prev_state = curr;
    a_sm->m_curr_state = next;
    return (curr != next.m_state);
}

SmState_t SmYieldSentinel(StateMachine_t *a_sm)
{
    SM_ASSERT(0);  // Should never get here.
}
