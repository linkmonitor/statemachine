#include "statemachine.h"

#include <assert.h>
#include <string.h>

void StateMachineInit(StateMachine_t *a_sm, SmRawState_t a_start_state,
                      void *a_context)
{
    assert(a_sm != NULL);
    assert(a_start_state != NULL);
    memset(a_sm, 0, sizeof(StateMachine_t));
    a_sm->m_curr_state.m_state = a_start_state;
    a_sm->m_context            = a_context;
}

bool StateMachineRun(StateMachine_t *a_sm)
{
    assert(a_sm != NULL);
    SmRawState_t curr = a_sm->m_curr_state.m_state;
    assert(curr != NULL);
    a_sm->m_next_state = NULL;
    SmState_t next     = curr(a_sm);
    if (next.m_state == NULL) next.m_state = curr;  // Yield case.
    a_sm->m_prev_state = curr;
    a_sm->m_curr_state = next;
    return (curr != next.m_state);
}
