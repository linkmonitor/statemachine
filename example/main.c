#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "statemachine.h"

typedef struct
{
    time_t m_entry_time;
} Context_t;

static SmState_t YellowLight(StateMachine_t *a_sm)
{
    SM_ENTRY(a_sm) printf("YellowLight:ENTRY\n");
    printf("YellowLight:BODY\n");
    SM_YIELD();
    SM_EXIT(a_sm) printf("YellowLight:EXIT\n");
}

static SmState_t RedLight(StateMachine_t *a_sm)
{
    Context_t *context = (Context_t *)a_sm->m_context;

    SM_ENTRY(a_sm)
    {
        printf("RedLight:ENTRY\n");
        time(&context->m_entry_time);  // Track when this state was entered.
    }

    printf("RedLight:BODY\n");

    time_t now;
    time(&now);
    bool timeout = difftime(now, context->m_entry_time) > 3.f /*seconds*/;

    SM_TRANSITION_IF(a_sm, timeout, YellowLight);
    SM_YIELD();

    SM_EXIT(a_sm) printf("RedLight:EXIT\n");
}

int main(void)
{
    Context_t context = {0};
    StateMachine_t sm;
    StateMachineInit(&sm, RedLight, &context);

    while (1)
    {
        if (sm.m_curr_state.m_state == YellowLight) break;
        while (StateMachineRun(&sm))
            ;
        sleep(1);
    }

    return 0;
}
