# Technical Overview

This project implements **Extended Position Based Dynamics (XPBD)**, a constraint-based framework for robust and efficient real-time simulation of deformable objects. Unlike force-based methods, XPBD defines object behavior through *geometric constraints* that enforce desired relationships between particles.

### Constraint Formulation

In XPBD, constraints are scalar functions that describe relationships between particle positions:

$$C(\mathbf{x}_0, \mathbf{x}_1, ..., \mathbf{x}_n)$$

where $\mathbf{x}_i \in \mathbb{R}^3$ represents particle positions. A constraint is satisfied when above expression equals zero. The framework iteratively corrects particle positions and updates Lagrange multipliers $\lambda$ to enforce constraints, enabling physically plausible deformation while maintaining stability under large time steps.

### Constraint Types

This simulator supports two primary constraint types:

- **Distance Constraints** for maintaining fixed edge lengths between particles which is defined as:

$$C_j(\mathbf{x}_k, \mathbf{x}_l) = |\mathbf{x}_l - \mathbf{x}_k| - d_0,$$

where $d_0$ is the rest distance.


- A **Volume Constraint** for preserving the objects volume which is defined as:

$$C(\mathbf{x}_0, ..., \mathbf{x}_{n-1}) = \left( \sum^{n_\text{T} - 1}_{i = 1} \frac{1}{6} (\mathbf{x}_{t_{i,0}} \times \mathbf{x}_{t_{i,1}}) \cdot \mathbf{x}_{t_{i,2}} \right) - pV_0,$$

where $n_\text{T}$ is the triangle count, $t_{i,0}, t_{i,1},$ and $t_{i,0}$ are the three vertex indices belonging to triangle $i$, $p$ is the overpressure factor, and $V_0$ is the rest volume.

### Simulation Loop

1. **Time Step Subdivision:**

To enable stable simulation under large time steps, the frame time $\Delta t$ is subdivided into $n_\text{steps}$ smaller substeps

$$\Delta t_s = \frac{\Delta t}{n_\text{steps}},$$

which accelerates constraint convergence and eliminates the need to explicitly track Lagrange multipliers $\lambda_j$ across time steps.

2. **Position Prediction:**

At each substep, particle positions are predicted by integrating external forces (e.g., gravity):

$$\tilde{\mathbf{x}} = \mathbf{x}^n + \Delta t_s \mathbf{v}^n + \Delta t_s^2 \mathbf{M}^{-1} \mathbf{f}_\text{ext}$$

3. **Constraint Solving:**

For each substep, all constraints are iteratively enforced by computing the position and Lagrange multiplier corrections:

$$\Delta \mathbf{x} = \mathbf{M}^{-1} \nabla C_j (\mathbf{x})^T \Delta \lambda_j,$$

$$\Delta \mathbf{\lambda}_j = \frac{-C_j(\mathbf{x}) - \gamma_j \nabla C_j (\mathbf{x} - \mathbf{x}^n)}{(1 + \gamma_j) \nabla C_j \mathbf{M}^{-1} \nabla C_j^T + \tilde{\alpha}_j},$$

where

- $\mathbf{M}$ is the mass matrix
- $\tilde{\alpha}_j$ is the inverse stiffness or *compliance*
- $\gamma_j = \frac{\tilde{\alpha}_j \tilde{\beta}_j}{\Delta t_s}$ is the time step scaled
product of compliance and damping parameter $\tilde{\beta}_j$.

Positions are then updated: $\mathbf{x}^{n+1} \leftarrow \tilde{\mathbf{x}} + \Delta \mathbf{x}$

4. **Velocity Update:**

After all constraints are satisfied, velocities are updated for the next substep:

$$\mathbf{v}^{n+1} = \frac{\mathbf{x}^{n+1} - \mathbf{x}^n}{\Delta t_s}.$$

This process repeats for each substep until the full frame time $\Delta t$ is consumed.
