import { inject, TestBed } from '@angular/core/testing';

import { SeerConnectionService } from './seer-connection.service';

describe('SeerConnectionService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [SeerConnectionService]
    });
  });

  it('should be created', inject([SeerConnectionService], (service: SeerConnectionService) => {
    expect(service).toBeTruthy();
  }));
});
