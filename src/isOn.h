/*!
 * @file
 * @author Jackson McNeill
 * Simply holds a bool for whether the engine is on or off. Do not use engine functions if it is false. You generally don't need to check this or anything, it is used by the engine internally.
 * Follows the IAMCOEDF-324342.1 Interplanetary Standard for weather something is ON or OFF (certified)
 */
#pragma once

extern bool GE_IsOn;
