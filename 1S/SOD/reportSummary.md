## 1. Presto: Uma Década de SQL Analytics na Meta

Este report descreve a evolução do **Presto** (agora conhecido como **Trino**), um motor SQL distribuído *open-source* criado pela **Meta (Facebook)** em 2012, essencial para a sua infraestrutura de dados em **escala de exabytes**.

---

### Arquitetura Inicial e Limitações

**Estrutura:**  
Dependente de uma arquitetura tradicional com **Coordenador único** e **Java Workers**.

**Limitações:**

- **Latência:** Gargalos de I/O devido à dependência de armazenamento remoto (*HDFS*) e à sobrecarga do *Garbage Collection* dos *Java workers*.  
- **Escalabilidade e Resiliência:** O coordenador único representava um ponto único de falha.  
  A execução era limitada pela **RAM disponível** (*in-memory*), tornando inviável a execução de queries muito grandes ou de longa duração.
- **Flexibilidade de Dados:** Incapaz de lidar com dados mutáveis e versionados, essenciais para aplicações modernas de **privacidade** e **Machine Learning**.

---

### Evolução da Arquitetura

Para enfrentar os desafios de **escalabilidade** e **latência** em escala de **exabytes**, a arquitetura do Presto evoluiu em **duas fases principais**:

---

#### Segunda Arquitetura — Disagregada e Elástica

Esta fase focou em **separar a computação do armazenamento** e **maximizar a performance interativa**.

**Disagregação e Escalabilidade:**

- Introduziu **múltiplos Coordenadores**, eliminando o ponto único de falha.  
- Implementou **Workers elásticos e sem estado**, permitindo que fossem adicionados ou removidos dinamicamente.  
- A **alocação de recursos** foi desmembrada do ciclo de vida da query, aumentando a flexibilidade e eficiência do cluster.

**Aceleração de Performance:**

- **Hierarchical Caching:**  
  Adotou *caching* SSD local (*Flash Cache*), reduzindo drasticamente a latência de I/O.
- **Motor Velox (C++):**  
  O principal avanço técnico. O **Velox** é um motor de execução de queries em **C++** com **execução vetorizada (SIMD)**.  
  Ao migrar a parte mais intensiva da execução do **Java** para **C++**, a Meta obteve **ganhos de performance de até duas ordens de magnitude** e reduziu a sobrecarga de *Garbage Collection*.

Apesar das melhorias, esta arquitetura **não garantiu tolerância a falhas suficiente** nem robustez para **cargas de trabalho ETL de longa duração**, além de **não suportar dados mutáveis** exigidos por aplicações modernas de **privacidade e Machine Learning**.

---

#### Terceira Arquitetura — Presto on Spark

A fase mais recente buscou **unificação** e **robustez** para cargas de trabalho de longa duração.

**Unificação da Plataforma:**

Desde **2022**, a **Meta** unificou o **Presto** e o **SparkSQL** sob uma arquitetura que atua como um **motor unificado de analytics (Single Piece Engine)**.

**Presto como Biblioteca:**

O **Presto** passou a ser executado **como uma biblioteca dentro do framework Spark**, o que proporcionou:

- **Tolerância a Falhas:**  
  Aproveita o sistema de recuperação (*recovery*) e gestão de estado (*RDDs*) do Spark, resolvendo falhas em longas execuções (como ETLs).  
- **Flexibilidade de Dados:**  
  Suporte nativo a **dados mutáveis e versionados**, essenciais para requisitos modernos de **privacidade** e **Machine Learning**, permitindo **atualizações e exclusões eficientes**.

---

### Conclusão

A combinação dessas evoluções permitiu ao **Presto** tornar-se um dos **motores SQL distribuídos mais versáteis** da atualidade.  
Hoje, ele atende desde **queries interativas de baixa latência** até **jobs ETL de longa duração**, consolidando-se como a **espinha dorsal do data warehouse da Meta**.
